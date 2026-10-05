//
//  Transcriber.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-10-05.
//

import Foundation
import Speech
import SwiftUI
import Synchronization
import os

// Many parts of this class were adapted from:
// - https://developer.apple.com/documentation/speech/bringing-advanced-speech-to-text-capabilities-to-your-app

public enum TranscriptionError: Error {
  case couldNotDownloadModel
  case failedToSetupRecognitionStream
  case invalidAudioDataType
  case localeNotSupported
  case noInternetForModelDownload
  case couldNotCaptureMicrophone
  case couldNotGenerateAudioProvider
  case micPermissionDenied

  var descriptionString: String {
    switch self {
    case .couldNotDownloadModel:
      return "Could not download the model."
    case .failedToSetupRecognitionStream:
      return "Could not set up the speech recognition stream."
    case .invalidAudioDataType:
      return "Unsupported audio format."
    case .localeNotSupported:
      return "This locale is not yet supported by SpeechAnalyzer."
    case .noInternetForModelDownload:
      return "The model could not be downloaded because the user is not connected to internet."
    case .couldNotCaptureMicrophone:
      return "Couldn't capture microphone device for voice recording."
    case .couldNotGenerateAudioProvider:
      return "Couldn't generate capture audio provider compatible with SpeechAnalyzer modules."
    case .micPermissionDenied:
      return "Microphone permission was not granted to use speech recognition."
    }
  }
}

class BufferConverter {
  enum Error: Swift.Error {
    case failedToCreateConverter
    case failedToCreateConversionBuffer
    case conversionFailed(NSError?)
  }

  private var converter: AVAudioConverter?
  func convertBuffer(_ buffer: AVAudioPCMBuffer, to format: AVAudioFormat) throws
    -> AVAudioPCMBuffer
  {
    let inputFormat = buffer.format
    guard inputFormat != format else {
      return buffer
    }

    if converter == nil || converter?.outputFormat != format {
      converter = AVAudioConverter(from: inputFormat, to: format)
      converter?.primeMethod = .none  // Sacrifice quality of first samples in order to avoid any timestamp drift from source
    }

    guard let converter else {
      throw Error.failedToCreateConverter
    }

    let sampleRateRatio = converter.outputFormat.sampleRate / converter.inputFormat.sampleRate
    let scaledInputFrameLength = Double(buffer.frameLength) * sampleRateRatio
    let frameCapacity = AVAudioFrameCount(scaledInputFrameLength.rounded(.up))
    guard
      let conversionBuffer = AVAudioPCMBuffer(
        pcmFormat: converter.outputFormat, frameCapacity: frameCapacity)
    else {
      throw Error.failedToCreateConversionBuffer
    }

    var nsError: NSError?
    var bufferProcessed = false

    let status = converter.convert(to: conversionBuffer, error: &nsError) {
      packetCount, inputStatusPointer in
      defer { bufferProcessed = true }  // This closure can be called multiple times, but it only offers a single buffer.
      inputStatusPointer.pointee = bufferProcessed ? .noDataNow : .haveData
      return bufferProcessed ? nil : buffer
    }

    guard status != .error else {
      throw Error.conversionFailed(nsError)
    }

    return conversionBuffer
  }
}

@Observable
final class Transcriber {
  private var inputSequence: AsyncStream<AnalyzerInput>?
  private var inputBuilder: AsyncStream<AnalyzerInput>.Continuation?
  private var transcriber: DictationTranscriber?
  private var analyzer: SpeechAnalyzer?
  private var recognizerTask: Task<(), Error>?

  var analyzerFormat: AVAudioFormat?

  var converter = BufferConverter()
  var downloadProgress: Progress?

  var transcript: String = ""
  var audioLevel: Float = 0.0
  var isTranscribing: Bool = false
  var downloadState: DownloadState = .idle

  enum DownloadState {
    case idle
    case inProgress
    case cancelled
    case finished
  }

  func setUpTranscriber(locales: [Locale]) async throws {
    let preset = DictationTranscriber.Preset.progressiveShortDictation
    var localeCandidate: Locale? = nil
    for l in locales {
      if await supported(locale: l) {
        localeCandidate = l
        break
      }
    }

    guard let locale = localeCandidate else {
      throw TranscriptionError.localeNotSupported
    }

    transcriber = DictationTranscriber(
      locale: locale,
      contentHints: preset.contentHints,
      transcriptionOptions: preset.transcriptionOptions.subtracting(
        [DictationTranscriber.TranscriptionOption.punctuation]),
      reportingOptions: preset.reportingOptions,
      attributeOptions: preset.attributeOptions
    )

    guard let transcriber else {
      throw TranscriptionError.failedToSetupRecognitionStream
    }

    analyzer = SpeechAnalyzer(modules: [transcriber])

    do {
      try await ensureModel(transcriber: transcriber, locale: locale)
    } catch let error as TranscriptionError {
      logger.error("\(error)")
      return
    }

    self.analyzerFormat = await SpeechAnalyzer.bestAvailableAudioFormat(compatibleWith: [
      transcriber
    ])
    (inputSequence, inputBuilder) = AsyncStream<AnalyzerInput>.makeStream()

    guard let inputSequence else { return }

    isTranscribing = true
    recognizerTask = Task { @MainActor in
      do {
        for try await case let result in transcriber.results {
          let text = result.text
          if result.isFinal {
            transcript += text.characters
          }
        }
      } catch {
        logger.error("Speech recognition failed")
      }
    }

    try await analyzer?.start(inputSequence: inputSequence)
  }

  func streamAudioToTranscriber(_ buffer: AVAudioPCMBuffer) async throws {
    guard let inputBuilder, let analyzerFormat else {
      throw TranscriptionError.invalidAudioDataType
    }

    let converted = try self.converter.convertBuffer(buffer, to: analyzerFormat)
    let input = AnalyzerInput(buffer: converted)
    inputBuilder.yield(input)
  }

  @MainActor func setAudioLevel(_ db: Float) {
    audioLevel = db
  }

  public func finishTranscribing() async throws {
    inputBuilder?.finish()
    try await analyzer?.finalizeAndFinishThroughEndOfInput()
    try await recognizerTask?.value
    recognizerTask?.cancel()
    recognizerTask = nil
    isTranscribing = false
  }

  func resetTranscription() {
    transcript = ""
  }
}

extension Transcriber {
  public func ensureModel(transcriber: DictationTranscriber, locale: Locale) async throws {
    guard await supported(locale: locale) else {
      throw TranscriptionError.localeNotSupported
    }

    if !(await AssetInventory.reservedLocales
      .map({ $0.identifier(.bcp47) }).contains(locale.identifier(.bcp47)))
    {
      _ = try await AssetInventory.reserve(locale: locale)
    }
    if !(await installed(locale: locale)) {
      try await downloadIfNeeded(for: transcriber)
    }
  }

  func supported(locale: Locale) async -> Bool {
    let supported = await DictationTranscriber.supportedLocales
    return supported.map { $0.identifier(.bcp47) }.contains(locale.identifier(.bcp47))
  }

  func installed(locale: Locale) async -> Bool {
    let installed = await Set(DictationTranscriber.installedLocales)
    return installed.map { $0.identifier(.bcp47) }.contains(locale.identifier(.bcp47))
  }

  func downloadIfNeeded(for module: DictationTranscriber) async throws {
    if let downloader = try await AssetInventory.assetInstallationRequest(supporting: [module]) {
      downloadState = .inProgress
      self.downloadProgress = downloader.progress
      try await downloader.downloadAndInstall()
      downloadState = .finished
    }
  }

  func releaseLocales() async {
    let reserved = await AssetInventory.reservedLocales
    for locale in reserved {
      await AssetInventory.release(reservedLocale: locale)
    }
  }
}

class Recorder: @unchecked Sendable {
  nonisolated(unsafe) private var outputContinuation: AsyncStream<SendableBuffer>.Continuation? =
    nil
  private let audioEngine: AVAudioEngine
  private let transcriber: Transcriber
  var playerNode: AVAudioPlayerNode?

  let firstSilenceTimeout: TimeInterval = 3
  let silenceTimeout: TimeInterval = 1
  let silenceThreshold: Float = 0.005

  nonisolated(unsafe) private var recordingStarted = Date()
  nonisolated(unsafe) private var lastSpeech = Date()
  nonisolated(unsafe) private var lastLevelUpdate = Date()
  nonisolated(unsafe) private var didTimeout = false

  init(transcriber: Transcriber) {
    audioEngine = AVAudioEngine()
    self.transcriber = transcriber
  }

  func start(locales: [Locale]) async throws {
    guard await isAuthorized() else {
      logger.error("User denied mic permission")
      throw TranscriptionError.micPermissionDenied
    }
    await transcriber.releaseLocales()
    #if os(iOS)
      try setUpAudioSession()
    #endif
    try await transcriber.setUpTranscriber(locales: locales)
    transcriber.resetTranscription()

    lastLevelUpdate = Date()
    recordingStarted = Date()
    lastSpeech = recordingStarted
    didTimeout = false

    logger.info("Started recording")

    for await wrapped in try await audioStream() {
      try await transcriber.streamAudioToTranscriber(wrapped.buffer)
    }
  }

  func stop() async throws {
    try await transcriber.finishTranscribing()
    audioEngine.stop()
  }

  #if os(iOS)
    nonisolated func setUpAudioSession() throws {
      let audioSession = AVAudioSession.sharedInstance()
      try audioSession.setCategory(.record, mode: .measurement)
      try audioSession.setActive(true, options: .notifyOthersOnDeactivation)
    }
  #endif

  nonisolated func rms(of buffer: SendableBuffer) -> Float {
    guard let ch = buffer.buffer.floatChannelData else { return 0 }
    let n = Int(buffer.buffer.frameLength)
    guard n > 0 else { return 0 }
    var sum: Float = 0
    for i in 0..<n {
      let s = ch[0][i]
      sum += s * s
    }
    return sqrt(sum / Float(n))
  }

  nonisolated private func audioStream() async throws -> AsyncStream<SendableBuffer> {
    try await setupAudioEngine()
    audioEngine.inputNode.installTap(
      onBus: 0,
      bufferSize: 1024,
      format: audioEngine.inputNode.outputFormat(forBus: 0)
    ) { [weak self] buffer, _ in
      guard let self else { return }
      guard let copy = AVAudioPCMBuffer(pcmFormat: buffer.format, frameCapacity: buffer.frameLength)
      else { return }

      copy.frameLength = buffer.frameLength

      let src = UnsafeMutableAudioBufferListPointer(buffer.mutableAudioBufferList)
      let dst = UnsafeMutableAudioBufferListPointer(copy.mutableAudioBufferList)

      for (s, d) in zip(src, dst) {
        guard let sp = s.mData, let dp = d.mData else { continue }
        dp.copyMemory(from: sp, byteCount: min(Int(s.mDataByteSize), Int(d.mDataByteSize)))
      }

      let level = rms(of: SendableBuffer(buffer: copy))

      if Date().timeIntervalSince(lastLevelUpdate) > 0.1 {
        lastLevelUpdate = Date()
        Task { @MainActor in
          transcriber.setAudioLevel(level / silenceThreshold)
        }
      }

      if level > silenceThreshold {
        lastSpeech = Date()
      } else if !didTimeout,
        recordingStarted.timeIntervalSince(lastSpeech) == 0,
        Date().timeIntervalSince(recordingStarted) > firstSilenceTimeout
      {
        didTimeout = true
        Task {
          logger.info("Stopping due to first silence timeout")
          try? await self.stop()
        }
      } else if !didTimeout,
        lastSpeech.timeIntervalSince(recordingStarted) > 0.5,
        Date().timeIntervalSince(lastSpeech) > silenceTimeout
      {
        didTimeout = true
        Task {
          logger.info("Stopping due to silence timeout")
          try? await self.stop()
        }
      }

      self.outputContinuation?.yield(SendableBuffer(buffer: copy))
    }

    audioEngine.prepare()
    try audioEngine.start()

    return AsyncStream(SendableBuffer.self, bufferingPolicy: .unbounded) {
      continuation in
      outputContinuation = continuation
    }
  }

  private func setupAudioEngine() throws {
    _ = audioEngine.inputNode.inputFormat(forBus: 0).settings
    audioEngine.inputNode.removeTap(onBus: 0)
  }

}

extension Recorder {
  func isAuthorized() async -> Bool {
    if AVCaptureDevice.authorizationStatus(for: .audio) == .authorized {
      return true
    }

    return await AVCaptureDevice.requestAccess(for: .audio)
  }
}

// For sending stuff across actor boundaries
nonisolated struct SendableBuffer: @unchecked Sendable {
  let buffer: AVAudioPCMBuffer
}

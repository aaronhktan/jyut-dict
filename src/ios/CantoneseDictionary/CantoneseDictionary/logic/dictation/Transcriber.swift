//
//  Transcriber.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-10-05.
//

import AVFoundation
import SwiftUI
import os

// Many parts of this class were adapted from https://github.com/JamesDale/Edu-MicLevel/blob/master/MicLevel/AudioCapture.swift
@Observable
final class Transcriber: NSObject {
  var audioLevel: Float = -40.0
  var transcribedSpeech: String = ""

  nonisolated(unsafe) private let captureSession = AVCaptureSession()
  private let sessionQueue: DispatchQueue = DispatchQueue(label: "MicSessionQueue")
  @ObservationIgnored nonisolated(unsafe) private var isCaptureSessionConfigured = false
  @ObservationIgnored nonisolated(unsafe) private var audioDeviceInput: AVCaptureDeviceInput?
  @ObservationIgnored nonisolated(unsafe) private var audioOutput: AVCaptureAudioDataOutput?
  @ObservationIgnored nonisolated(unsafe) private var audioConnection: AVCaptureConnection?

  @ObservationIgnored nonisolated(unsafe) private var audioCaptureDevice: AVCaptureDevice? {
    didSet {
      guard let captureDevice = audioCaptureDevice else {
        return
      }
      logger.debug("Using capture device: \(captureDevice.localizedName)")
      let id = captureDevice.uniqueID
      sessionQueue.async { [self] in
        guard let dev = AVCaptureDevice(uniqueID: id) else { return }
        self.updateSessionForCaptureDevice(dev)
      }
    }
  }

  public var isRunning: Bool {
    captureSession.isRunning
  }

  override init() {
    super.init()
    audioCaptureDevice = getAudioCaptureDevice()
  }

  private func getAudioCaptureDevice() -> AVCaptureDevice? {
    AVCaptureDevice.default(for: .audio)
  }

  nonisolated private func deviceInputFor(device: AVCaptureDevice?) -> AVCaptureDeviceInput? {
    guard let validDevice = device else {
      return nil
    }
    do {
      return try AVCaptureDeviceInput(device: validDevice)
    } catch let error {
      logger.error("Error getting capture device input: \(error.localizedDescription)")
      return nil
    }
  }

  nonisolated private func updateSessionForCaptureDevice(_ captureDevice: AVCaptureDevice) {
    guard isCaptureSessionConfigured else {
      return
    }

    captureSession.beginConfiguration()
    defer {
      captureSession.commitConfiguration()
    }

    for input in captureSession.inputs {
      if let deviceInput = input as? AVCaptureDeviceInput {
        captureSession.removeInput(deviceInput)
      }
    }

    if let deviceInput = deviceInputFor(device: captureDevice) {
      if !captureSession.inputs.contains(deviceInput), captureSession.canAddInput(deviceInput) {
        captureSession.addInput(deviceInput)
      }
    }
  }

  nonisolated func configureCaptureSession(completionHandler: (_ success: Bool) -> Void) {
    var success = false
    self.captureSession.beginConfiguration()

    defer {
      self.captureSession.commitConfiguration()
      completionHandler(success)
    }

    guard let audioCaptureDevice = audioCaptureDevice,
      let audioDeviceInput = try? AVCaptureDeviceInput(device: audioCaptureDevice)
    else {
      logger.error("Failed to obtain audio input")
      return
    }

    let audioOutput = AVCaptureAudioDataOutput()
    audioOutput.setSampleBufferDelegate(self, queue: DispatchQueue(label: "AudioDataOutputQueue"))

    guard captureSession.canAddInput(audioDeviceInput) else {
      logger.error("Unable to add audio device input to capture session")
      return
    }

    guard captureSession.canAddOutput(audioOutput) else {
      logger.error("Unable to add audio output to capture session")
      return
    }

    captureSession.addInput(audioDeviceInput)
    captureSession.addOutput(audioOutput)

    self.audioDeviceInput = audioDeviceInput
    self.audioOutput = audioOutput
    self.audioConnection = audioOutput.connection(with: .audio)

    isCaptureSessionConfigured = true
    success = true
  }

  private func checkMicAuthorization() async -> Bool {
    switch AVCaptureDevice.authorizationStatus(for: .audio) {
    case .authorized:
      logger.info("Microphone access authorized")
      return true
    case .notDetermined:
      logger.info("Microphone access not yet determined")
      sessionQueue.suspend()
      let status = await AVCaptureDevice.requestAccess(for: .audio)
      sessionQueue.resume()
      return status
    case .denied:
      logger.info("Microphone access denied")
      return false
    case .restricted:
      logger.info("Microphone access restricted")
      return false
    default:
      return false
    }
  }

  func start() async -> Bool {
    let authorized = await checkMicAuthorization()
    guard authorized else {
      logger.error("Did not have access to microphone")
      return false
    }

    if isCaptureSessionConfigured {
      if !captureSession.isRunning {
        sessionQueue.async { [self] in
          self.captureSession.startRunning()
        }
      }
      return true
    }

    sessionQueue.async { [self] in
      self.configureCaptureSession { success in
        guard success else {
          return
        }
        self.captureSession.startRunning()
        return
      }
    }

    return true
  }

  func stop() {
    guard isCaptureSessionConfigured else {
      return
    }

    if captureSession.isRunning {
      sessionQueue.async {
        self.captureSession.stopRunning()
      }
    }
  }
}

extension Transcriber: AVCaptureVideoDataOutputSampleBufferDelegate,
  AVCaptureAudioDataOutputSampleBufferDelegate
{

  nonisolated public func captureOutput(
    _ output: AVCaptureOutput,
    didOutput sampleBuffer: CMSampleBuffer,
    from connection: AVCaptureConnection
  ) {
    let avgFloat = audioConnection?.audioChannels
      .compactMap { $0.averagePowerLevel }
      .reduce(0, +)

    if let avgFloat = avgFloat {
      Task { @MainActor [weak self] in
        self?.audioLevel = avgFloat
      }
    }
  }
}

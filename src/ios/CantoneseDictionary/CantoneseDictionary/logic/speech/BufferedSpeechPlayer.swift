//
//  BufferedSpeechPlayer.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-10-17.
//

import AVFoundation
import os

@MainActor
final class BufferedSpeechPlayer {
  private let synthesizer = AVSpeechSynthesizer()
  private let engine = AVAudioEngine()
  private let player = AVAudioPlayerNode()
  private var buffers: [AVAudioPCMBuffer] = []
  private var isGenerating = false
  private var numBuffersRemaining = 0

  init() {
    engine.attach(player)
    synthesizer.usesApplicationAudioSession = true
  }

  func speak(_ utterance: AVSpeechUtterance) async {
    // Must set category for audio session in order to get playback
    // working correctly on AirPods and internal speakers
    #if os(iOS)
      let audioSession = AVAudioSession.sharedInstance()
      do {
        try audioSession.setCategory(
          .playback, mode: .voicePrompt, options: [.duckOthers, .mixWithOthers])
        if #available(iOS 27, *) {
          try await audioSession.activate()
        } else {
          try audioSession.setActive(true)
        }
      } catch {
        logger.error("Failed to activate audio session, \(error)")
        return
      }
    #endif

    stop()
    isGenerating = true

    // Writing to buffers first, and then queueing for playback removes
    // clicking when starting playback on AirPods
    synthesizer.write(utterance) { [weak self] buffer in
      guard let pcm = buffer as? AVAudioPCMBuffer else {
        logger.error("Could not cast utterance buffer to PCM!")
        return
      }

      // A zero-length buffer signals that generation is finished
      let copy = pcm.frameLength > 0 ? pcm.deepCopy() : nil

      Task { @MainActor in
        guard let self, self.isGenerating else {
          return
        }

        if let copy {
          self.buffers.append(copy)
        } else if pcm.frameLength == 0 {
          self.finishGenerating()
        }
      }
    }
  }

  func stop() {
    synthesizer.stopSpeaking(at: .immediate)
    player.stop()
    buffers.removeAll()
    isGenerating = false
  }

  private func finishGenerating() {
    isGenerating = false
    guard let format = buffers.first?.format else {
      logger.warning("No buffer was generated when finishGenerating() was called")
      return
    }

    do {
      player.stop()
      engine.stop()

      // This is necessary so that the playback format matches the buffer format
      engine.disconnectNodeOutput(player)
      engine.connect(player, to: engine.mainMixerNode, format: format)

      try engine.start()

      numBuffersRemaining = buffers.count
      for b in buffers {
        player.scheduleBuffer(
          b,
          completionCallbackType: .dataPlayedBack,
          completionHandler: { _ in
            // The completion handler only checks that _one_ buffer's playback is complete
            // So we need to keep track of the total number of buffers, and deactivate the
            // audio session once the number of buffers completed matches the total number
            // of buffers
            Task { @MainActor in
              self.numBuffersRemaining -= 1
              if self.numBuffersRemaining == 0 {
                #if os(iOS)
                  let audioSession = AVAudioSession.sharedInstance()
                  do {
                    if #available(iOS 27, *) {
                      try await audioSession.deactivate()
                    } else {
                      try audioSession.setActive(false, options: .notifyOthersOnDeactivation)
                    }
                  } catch {
                    logger.error("Failed to deactivate audio session, \(error)")
                    return
                  }
                #endif
              }
            }
          })
      }
      buffers.removeAll()
      player.play()
    } catch {
      logger.error("Could not start speech audio engine: \(error)")
    }
  }
}

extension AVAudioPCMBuffer {
  fileprivate func deepCopy() -> AVAudioPCMBuffer? {
    guard let dest = AVAudioPCMBuffer(pcmFormat: format, frameCapacity: frameLength) else {
      return nil
    }
    dest.frameLength = frameLength

    let src = UnsafeMutableAudioBufferListPointer(mutableAudioBufferList)
    let dst = UnsafeMutableAudioBufferListPointer(dest.mutableAudioBufferList)
    for (s, d) in zip(src, dst) {
      guard let sData = s.mData, let dData = d.mData else {
        return nil
      }
      memcpy(dData, sData, Int(s.mDataByteSize))
    }
    return dest
  }
}

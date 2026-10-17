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

  init() {
    engine.attach(player)
    synthesizer.usesApplicationAudioSession = true
  }

  func speak(_ utterance: AVSpeechUtterance) {
    stop()
    isGenerating = true

    synthesizer.write(utterance) { [weak self] buffer in
      guard let pcm = buffer as? AVAudioPCMBuffer else {
        logger.error("Could not cast utterance buffer to PCM!")
        return
      }

      // A zero-length buffer signals that generation is finished.
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

      for b in buffers {
        player.scheduleBuffer(b)
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

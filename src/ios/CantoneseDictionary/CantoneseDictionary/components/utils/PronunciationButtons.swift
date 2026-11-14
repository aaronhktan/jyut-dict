//
//  PronunciationButtons.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-11-13.
//

import AVFoundation
import Intents
import SwiftUI
import os

struct SpeakErrorState: Error {
  var showAlert: Bool
  var error: String
}

struct CantonesePronunciationButton: View {
  @State private var animateCantoneseSpeaker = false

  var speakErrorState: Binding<SpeakErrorState>
  var player: Binding<BufferedSpeechPlayer>
  var showVoiceError: Binding<Bool>
  var words: () -> Result<String, SpeakErrorState>

  var body: some View {
    Button {
      animateCantoneseSpeaker.toggle()

      let result = words()
      if case .failure(let error) = result {
        speakErrorState.wrappedValue = error
        return
      }
      let utterance = AVSpeechUtterance(string: try! result.get())

      var voice = AVSpeechSynthesisVoice(identifier: "com.apple.voice.compact.zh-HK.Sinji")
      if voice == nil {
        for v in AVSpeechSynthesisVoice.speechVoices() {
          if v.language == "yue-HK" {
            voice = v
            break
          }
        }
      }
      guard let voice else {
        speakErrorState.wrappedValue = SpeakErrorState(
          showAlert: true, error: "Couldn't load a Cantonese voice!")
        logger.error("No Cantonese voice could be found!")
        return
      }
      utterance.voice = voice

      Task {
        await player.wrappedValue.setupAudioSession()
        await player.wrappedValue.speak(utterance)
      }
    } label: {
      Label("Pronounce in Cantonese", systemImage: "speaker.wave.2")
        .symbolEffect(.variableColor.iterative, value: animateCantoneseSpeaker)
    }
    .labelStyle(.iconOnly)
    .buttonBorderShape(.circle)
    .buttonStyle(.glass)
    .controlSize(.mini)
  }
}

struct MandarinPronunciationButton: View {
  @State private var animateMandarinSpeaker = false

  var speakErrorState: Binding<SpeakErrorState>
  var player: Binding<BufferedSpeechPlayer>
  var showVoiceError: Binding<Bool>
  var words: () -> Result<String, SpeakErrorState>

  var body: some View {
    Button {
      animateMandarinSpeaker.toggle()

      let result = words()
      if case .failure(let error) = result {
        speakErrorState.wrappedValue = error
        return
      }
      let utterance = AVSpeechUtterance(string: try! result.get())

      var voice: AVSpeechSynthesisVoice? = nil
      for v in AVSpeechSynthesisVoice.speechVoices() {
        if v.identifier == "com.apple.voice.compact.zh-TW.Meijia" {
          voice = v
          break
        }
        if v.language == "zh-TW", !v.identifier.contains("eloquence") {
          voice = v
        }
      }
      if voice == nil {
        for v in AVSpeechSynthesisVoice.speechVoices() {
          if v.language == "zh-CN", !v.identifier.contains("eloquence") {
            voice = v
            break
          }
        }
      }
      guard let voice else {
        speakErrorState.wrappedValue = SpeakErrorState(
          showAlert: true, error: "Couldn't load a Mandarin voice!")
        logger.error("No Mandarin voice could be found!")
        return
      }
      utterance.voice = voice

      if #unavailable(iOS 27) {
        INPreferences.requestSiriAuthorization { status in
          if status == .authorized {
            if ["zh-HK", "yue-CN"].contains(INPreferences.siriLanguageCode()) {
              showVoiceError.wrappedValue = true
              return
            }
          }

          Task {
            await player.wrappedValue.setupAudioSession()
            await player.wrappedValue.speak(utterance)
          }
        }
      } else {
        Task {
          await player.wrappedValue.setupAudioSession()
          await player.wrappedValue.speak(utterance)
        }
      }
    } label: {
      Label("Pronounce in Mandarin", systemImage: "speaker.wave.2")
        .symbolEffect(.variableColor.iterative, value: animateMandarinSpeaker)
    }
    .labelStyle(.iconOnly)
    .buttonBorderShape(.circle)
    .buttonStyle(.glass)
    .controlSize(.mini)
  }
}

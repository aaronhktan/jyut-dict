//
//  ExampleDetail.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-10-29.
//

import AVFoundation
import SwiftUI
import os

struct ExampleDetail: View {
  @Environment(Settings.self) private var settings

  let example: Example

  @State private var animateCantoneseSpeaker = false
  @State private var animateMandarinSpeaker = false
  @State private var speakErrorState: (showAlert: Bool, error: String) = (false, "")
  @State private var player = BufferedSpeechPlayer()

  var body: some View {
    List {
      VStack(alignment: .leading, spacing: 4) {
        Text(
          iso693ToLanguageName[example.sourceLanguage] ?? example.sourceLanguage
        )
        .padding(.horizontal, 6)
        .foregroundStyle(
          getContrastingColour(backgroundColour: languageColours[example.sourceLanguage] ?? .gray)
        )
        .background(
          Capsule()
            .fill(languageColours[example.sourceLanguage] ?? .gray)
        )
        .fixedSize(horizontal: false, vertical: true)

        characterDisplay()
        .font(.largeTitle)
        .frame(maxWidth: .infinity, alignment: .leading)
        .textSelection(.enabled)
        Grid {
          switch settings.previewPhoneticOptions {
          case .onlyCantonese, .preferCantonese:
            cantonesePronunciationView()
            mandarinPronunicationView()
          case .onlyMandarin, .preferMandarin:
            mandarinPronunicationView()
            cantonesePronunciationView()
          }
        }
      }
      .padding(.horizontal)
      .listRowSeparator(.hidden)
      .listRowBackground(Color.clear)
      .listRowInsets(EdgeInsets(top: 0, leading: 0, bottom: 10, trailing: 0))

      ForEach(example.getTranslationSets(), id: \.id) { set in
        TranslationCard(set: set)
          .listRowSeparator(.hidden)
          .listRowBackground(Color.clear)
          .listRowInsets(EdgeInsets(top: 5, leading: 0, bottom: 5, trailing: 0))
      }
    }
    .listStyle(.plain)
    .scrollContentBackground(.hidden)
    .alert(isPresented: $speakErrorState.showAlert) {
      Alert(
        title: Text("Couldn't speak example!"),
        message: Text(speakErrorState.error),
        dismissButton: .default(Text("OK")))
    }
  }
}

extension ExampleDetail {
  @ViewBuilder
  private func characterDisplay() -> some View {
    switch settings.entryCharactersOptions {
    case .onlyTraditional:
      Text(example.traditional)
        .fixedSize(horizontal: false, vertical: true)
        .textSelection(.enabled)
    case .onlySimplified:
      Text(example.simplified)
        .fixedSize(horizontal: false, vertical: true)
        .textSelection(.enabled)
    case .preferTraditional:
      Text(example.traditional)
        .fixedSize(horizontal: false, vertical: true)
        .textSelection(.enabled)
      Text(example.simplified)
        .fixedSize(horizontal: false, vertical: true)
        .textSelection(.enabled)
    case .preferSimplified:
      Text(example.simplified)
        .fixedSize(horizontal: false, vertical: true)
        .textSelection(.enabled)
      Text(example.traditional)
        .fixedSize(horizontal: false, vertical: true)
        .textSelection(.enabled)
    }
  }

  @ViewBuilder
  private func cantonesePronunciationView() -> some View {
    if settings.entryCantonesePhoneticOptions.contains(.rawJyutping) {
      GridRow(alignment: .firstTextBaseline) {
        cantonesePronunciationButton()

        HStack(alignment: .firstTextBaseline) {
          Text("Jyutping")
            .foregroundStyle(.placeholder)
          Text(
            example.getPhonetic(
              options: .onlyCantonese, cantoneseOptions: .rawJyutping,
              mandarinOptions: .prettyPinyin)
          )
          .padding(.trailing)
          .frame(maxWidth: .infinity, alignment: .leading)
          .fixedSize(horizontal: false, vertical: true)
          .textSelection(.enabled)
        }
      }
    }
    if settings.entryCantonesePhoneticOptions.contains(.prettyYale) {
      GridRow(alignment: .firstTextBaseline) {
        if !settings.entryCantonesePhoneticOptions.contains(.rawJyutping) {
          cantonesePronunciationButton()
        } else {
          Spacer()
        }

        HStack(alignment: .firstTextBaseline) {
          Text("Yale")
            .foregroundStyle(.placeholder)
          Text(
            example.getPhonetic(
              options: .onlyCantonese, cantoneseOptions: .prettyYale,
              mandarinOptions: .prettyPinyin)
          )
          .padding(.trailing)
          .frame(maxWidth: .infinity, alignment: .leading)
          .fixedSize(horizontal: false, vertical: true)
          .textSelection(.enabled)

        }
      }
    }
    if settings.entryCantonesePhoneticOptions.contains(.cantoneseIPA) {
      GridRow(alignment: .firstTextBaseline) {
        if !settings.entryCantonesePhoneticOptions.contains(.rawJyutping),
          !settings.entryCantonesePhoneticOptions.contains(.prettyYale)
        {
          cantonesePronunciationButton()
        } else {
          Spacer()
        }

        HStack(alignment: .firstTextBaseline) {
          Text("Cantonese IPA")
            .foregroundStyle(.placeholder)
          Text(
            example.getPhonetic(
              options: .onlyCantonese, cantoneseOptions: .cantoneseIPA,
              mandarinOptions: .prettyPinyin)
          )
          .padding(.trailing)
          .frame(maxWidth: .infinity, alignment: .leading)
          .fixedSize(horizontal: false, vertical: true)
          .textSelection(.enabled)
        }
      }
    }
  }

  @ViewBuilder
  private func cantonesePronunciationButton() -> some View {
    Button {
      animateCantoneseSpeaker.toggle()

      var words =
        example.getPhonetic(
          options: .onlyCantonese, cantoneseOptions: .rawJyutping,
          mandarinOptions: .prettyPinyin)
      if words.isEmpty {
        words = example.traditional
      }
      let utterance = AVSpeechUtterance(string: words)

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
        speakErrorState = (true, "Couldn't load a Cantonese voice!")
        logger.error("No Cantonese voice could be found!")
        return
      }
      utterance.voice = voice

      Task {
        await player.setupAudioSession()
        await player.speak(utterance)
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

  @ViewBuilder
  private func mandarinPronunicationView() -> some View {
    if settings.entryMandarinPhoneticOptions.contains(.prettyPinyin) {
      GridRow(alignment: .firstTextBaseline) {
        mandarinPronunciationButton()

        HStack(alignment: .firstTextBaseline) {
          Text("Pinyin")
            .foregroundStyle(.placeholder)
          Text(
            example.getPhonetic(
              options: .onlyMandarin, cantoneseOptions: .rawJyutping,
              mandarinOptions: .prettyPinyin)
          )
          .padding(.trailing)
          .frame(maxWidth: .infinity, alignment: .leading)
          .fixedSize(horizontal: false, vertical: true)
          .textSelection(.enabled)
        }
      }
    }
    if settings.entryMandarinPhoneticOptions.contains(.rawPinyin) {
      GridRow(alignment: .firstTextBaseline) {
        if !settings.entryMandarinPhoneticOptions.contains(.prettyPinyin) {
          mandarinPronunciationButton()
        } else {
          Spacer()
        }

        HStack(alignment: .firstTextBaseline) {
          Text("Raw Pinyin")
            .foregroundStyle(.placeholder)
          Text(
            example.getPhonetic(
              options: .onlyMandarin, cantoneseOptions: .rawJyutping,
              mandarinOptions: .rawPinyin)
          )
          .padding(.trailing)
          .frame(maxWidth: .infinity, alignment: .leading)
          .fixedSize(horizontal: false, vertical: true)
          .textSelection(.enabled)
        }
      }
    }
    if settings.entryMandarinPhoneticOptions.contains(.zhuyin) {
      GridRow(alignment: .firstTextBaseline) {
        if !settings.entryMandarinPhoneticOptions.contains(.prettyPinyin),
          settings.entryMandarinPhoneticOptions.contains(.rawPinyin)
        {
          mandarinPronunciationButton()
        } else {
          Spacer()
        }

        HStack(alignment: .firstTextBaseline) {
          Text("Zhuyin")
            .foregroundStyle(.placeholder)
          Text(
            example.getPhonetic(
              options: .onlyMandarin, cantoneseOptions: .rawJyutping,
              mandarinOptions: .zhuyin)
          )
          .padding(.trailing)
          .frame(maxWidth: .infinity, alignment: .leading)
          .fixedSize(horizontal: false, vertical: true)
          .textSelection(.enabled)
        }
      }
    }
    if settings.entryMandarinPhoneticOptions.contains(.mandarinIPA) {
      GridRow(alignment: .firstTextBaseline) {
        if !settings.entryMandarinPhoneticOptions.contains(.prettyPinyin),
          settings.entryMandarinPhoneticOptions.contains(.rawPinyin),
          settings.entryMandarinPhoneticOptions.contains(.zhuyin)
        {
          mandarinPronunciationButton()
        } else {
          Spacer()
        }

        HStack(alignment: .firstTextBaseline) {
          Text("Mandarin IPA")
            .foregroundStyle(.placeholder)
          Text(
            example.getPhonetic(
              options: .onlyMandarin, cantoneseOptions: .rawJyutping,
              mandarinOptions: .mandarinIPA)
          )
          .padding(.trailing)
          .frame(maxWidth: .infinity, alignment: .leading)
          .fixedSize(horizontal: false, vertical: true)
          .textSelection(.enabled)
        }
      }
    }
  }

  @ViewBuilder
  private func mandarinPronunciationButton() -> some View {
    Button {
      animateMandarinSpeaker.toggle()

      var words =
        example.getPhonetic(
          options: .onlyMandarin, cantoneseOptions: .rawJyutping,
          mandarinOptions: .rawPinyin)
      if words.isEmpty {
        words = example.traditional
      }
      let utterance = AVSpeechUtterance(string: words)

      var voice = AVSpeechSynthesisVoice(identifier: "com.apple.voice.compact.zh-TW.Meijia")
      if voice == nil {
        for v in AVSpeechSynthesisVoice.speechVoices() {
          if v.language == "zh-TW" {
            voice = v
            break
          }
        }
      }
      if voice == nil {
        for v in AVSpeechSynthesisVoice.speechVoices() {
          if v.language == "zh-CN" {
            voice = v
            break
          }
        }
      }
      guard let voice else {
        speakErrorState = (true, "Couldn't load a Mandarin voice!")
        logger.error("No Mandarin voice could be found!")
        return
      }
      utterance.voice = voice

      Task {
        await player.setupAudioSession()
        await player.speak(utterance)
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

//
//  ExampleDetail.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-10-29.
//

import AVFoundation
import Intents
import SwiftUI
import os

struct ExampleDetail: View {
  @Environment(Settings.self) private var settings
  @Environment(\.openURL) private var openURL

  let example: Example

  @State private var speakErrorState = SpeakErrorState(showAlert: false, error: "")
  @State private var showVoiceError = false
  @State private var player = BufferedSpeechPlayer()

  var body: some View {
    ScrollView {
      VStack(spacing: 10) {
        header()

        ForEach(example.getTranslationSets(), id: \.id) { set in
          TranslationCard(set: set)
            .listRowSeparator(.hidden)
            .listRowBackground(Color.clear)
            .listRowInsets(EdgeInsets(top: 5, leading: 0, bottom: 5, trailing: 0))
        }
      }
    }
    .speakErrorAlert(error: $speakErrorState)
    .voiceErrorAlert(isPresented: $showVoiceError)
  }
}

extension ExampleDetail {
  @ViewBuilder
  private func header() -> some View {
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
  }

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
        CantonesePronunciationButton(
          speakErrorState: $speakErrorState,
          player: $player,
          showVoiceError: $showVoiceError
        ) {
          getCantoneseWords()
        }

        HStack(alignment: .firstTextBaseline) {
          Text("Jyutping")
            .foregroundStyle(.placeholder)
          Text(
            example.getPhonetic(options: .onlyCantonese, cantoneseOptions: .rawJyutping)
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
          CantonesePronunciationButton(
            speakErrorState: $speakErrorState,
            player: $player,
            showVoiceError: $showVoiceError
          ) {
            getCantoneseWords()
          }
        } else {
          Spacer()
        }

        HStack(alignment: .firstTextBaseline) {
          Text("Yale")
            .foregroundStyle(.placeholder)
          Text(
            example.getPhonetic(options: .onlyCantonese, cantoneseOptions: .prettyYale)
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
          CantonesePronunciationButton(
            speakErrorState: $speakErrorState,
            player: $player,
            showVoiceError: $showVoiceError
          ) {
            getCantoneseWords()
          }
        } else {
          Spacer()
        }

        HStack(alignment: .firstTextBaseline) {
          Text("Cantonese IPA")
            .foregroundStyle(.placeholder)
          Text(
            example.getPhonetic(options: .onlyCantonese, cantoneseOptions: .cantoneseIPA)
          )
          .padding(.trailing)
          .frame(maxWidth: .infinity, alignment: .leading)
          .fixedSize(horizontal: false, vertical: true)
          .textSelection(.enabled)
        }
      }
    }
  }

  private func getCantoneseWords() -> Result<String, SpeakErrorState>{
    var words =
      example.getPhonetic(
        options: .onlyCantonese, cantoneseOptions: .rawJyutping)
    if words.isEmpty {
      words = example.traditional
    }
    return .success(words)
  }

  @ViewBuilder
  private func mandarinPronunicationView() -> some View {
    if settings.entryMandarinPhoneticOptions.contains(.prettyPinyin) {
      GridRow(alignment: .firstTextBaseline) {
        MandarinPronunciationButton(
          speakErrorState: $speakErrorState,
          player: $player,
          showVoiceError: $showVoiceError
        ) {
          getMandarinWords()
        }

        HStack(alignment: .firstTextBaseline) {
          Text("Pinyin")
            .foregroundStyle(.placeholder)
          Text(
            example.getPhonetic(options: .onlyMandarin, mandarinOptions: .prettyPinyin)
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
          MandarinPronunciationButton(
            speakErrorState: $speakErrorState,
            player: $player,
            showVoiceError: $showVoiceError
          ) {
            getMandarinWords()
          }
        } else {
          Spacer()
        }

        HStack(alignment: .firstTextBaseline) {
          Text("Raw Pinyin")
            .foregroundStyle(.placeholder)
          Text(
            example.getPhonetic(options: .onlyMandarin, mandarinOptions: .rawPinyin)
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
          MandarinPronunciationButton(
            speakErrorState: $speakErrorState,
            player: $player,
            showVoiceError: $showVoiceError
          ) {
            getMandarinWords()
          }
        } else {
          Spacer()
        }

        HStack(alignment: .firstTextBaseline) {
          Text("Zhuyin")
            .foregroundStyle(.placeholder)
          Text(
            example.getPhonetic(options: .onlyMandarin, mandarinOptions: .zhuyin)
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
          MandarinPronunciationButton(
            speakErrorState: $speakErrorState,
            player: $player,
            showVoiceError: $showVoiceError
          ) {
            getMandarinWords()
          }
        } else {
          Spacer()
        }

        HStack(alignment: .firstTextBaseline) {
          Text("Mandarin IPA")
            .foregroundStyle(.placeholder)
          Text(
            example.getPhonetic(options: .onlyMandarin, mandarinOptions: .mandarinIPA)
          )
          .padding(.trailing)
          .frame(maxWidth: .infinity, alignment: .leading)
          .fixedSize(horizontal: false, vertical: true)
          .textSelection(.enabled)
        }
      }
    }
  }

  private func getMandarinWords() -> Result<String, SpeakErrorState> {
    var words =
      example.getPhonetic(
        options: .onlyMandarin, cantoneseOptions: .rawJyutping,
        mandarinOptions: .rawPinyin)
    if words.isEmpty {
      words = example.traditional
    }
    return .success(words)
  }
}

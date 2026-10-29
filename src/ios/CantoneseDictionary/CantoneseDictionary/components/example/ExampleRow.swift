//
//  ExampleRow.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-10-29.
//

import SwiftUI

struct ExampleRow: View {
  @Environment(Settings.self) private var settings

  let example: Example

  @State private var phonetic = ""

  var body: some View {
    VStack(alignment: .leading) {
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

      Text(
        example.getCharacters(
          options: settings.entryCharactersOptions
        )
      )
      .font(.title)
      .rowLine()

      Text(phonetic)
      .foregroundStyle(.placeholder)
      .rowLine()
      
      Text(example.getTranslationSnippet())
        .rowLine()
    }
    .onAppear {
      phonetic = example.getPhonetic(
        options: settings.previewPhoneticOptions,
        cantoneseOptions: settings.previewCantonesePhoneticOptions,
        mandarinOptions: settings.previewMandarinPhoneticOptions
      )
      phonetic = phonetic.isEmpty ? "-" : phonetic
    }
  }
}

//
//  ExampleCard.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-26.
//

import SwiftUI

struct ExampleCard: View {
  @Environment(\.colorScheme) var colorScheme

  let source: String
  let examples: [Example]

  var body: some View {
    VStack(alignment: .leading) {
      Text("Examples (\(source))")
        .foregroundStyle(.placeholder)
        .frame(maxWidth: .infinity, alignment: .leading)
        .padding(.horizontal, 12)
        .padding(.vertical, 12)
        .background(
          UnevenRoundedRectangle(topLeadingRadius: 15, topTrailingRadius: 15)
            .fill(
              (colorScheme == .dark) ? AnyShapeStyle(.quinary) : AnyShapeStyle(.gray.opacity(0.1))
            )
        )
        .onTapGesture {
          #if os(iOS)
            // Makes deselecting text possible
            UIApplication.shared.sendAction(
              #selector(UIResponder.resignFirstResponder), to: nil, from: nil, for: nil)
          #endif
        }
      VStack(alignment: .leading) {
        ForEach(
          examples.enumerated(),
          id: \.element.id
        ) { index, example in
          HStack(alignment: .top, spacing: 5) {
            Text("\(index + 1)")
              .foregroundStyle(.placeholder)
              .frame(width: 20)

            ExampleView(example: example)
          }
        }
      }
      .padding(.top, 3)
      .padding(.horizontal, 12)
      .padding(.bottom)
    }
    .background {
      RoundedRectangle(cornerRadius: 15)
        .fill(
          (colorScheme == .dark) ? AnyShapeStyle(.quinary) : AnyShapeStyle(.gray.opacity(0.1))
        )
        .onTapGesture {
          // Makes deselecting text possible
          #if os(iOS)
            UIApplication.shared.sendAction(
              #selector(UIResponder.resignFirstResponder), to: nil, from: nil, for: nil)
          #endif
        }
    }
    .padding(.horizontal)
  }
}

private struct ExampleView: View {
  let example: Example

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

      Text(example.traditional)
        .fixedSize(horizontal: false, vertical: true)
        .textSelection(.enabled)
      Text(example.simplified)
        .fixedSize(horizontal: false, vertical: true)
        .textSelection(.enabled)

      if !example.jyutping.isEmpty {
        Text(example.jyutping)
          .fixedSize(horizontal: false, vertical: true)
          .foregroundStyle(.placeholder)
          .textSelection(.enabled)
      }
      if !example.pinyin.isEmpty {
        Text(example.getMandarinPhonetic(mandarinOptions: .prettyPinyin))
          .fixedSize(horizontal: false, vertical: true)
          .foregroundStyle(.placeholder)
          .textSelection(.enabled)
      }

      ForEach(example.getTranslationSets(), id: \.id) { translationSet in
        if let first = translationSet.getTranslations().first {
          Text(first.content)
            .fixedSize(horizontal: false, vertical: true)
            .textSelection(.enabled)
        }
      }
    }
  }
}

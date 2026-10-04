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
  @Environment(Settings.self) private var settings

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

      characterDisplay()
      pronunicationDisplay()

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

extension ExampleView {
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
  private func pronunicationDisplay() -> some View {
    switch settings.previewPhoneticOptions {
    case .onlyCantonese:
      cantonesePronunicationDisplay()
    case .onlyMandarin:
      mandarinPronunicationDisplay()
    case .preferCantonese:
      cantonesePronunicationDisplay()
      mandarinPronunicationDisplay()
    case .preferMandarin:
      mandarinPronunicationDisplay()
      cantonesePronunicationDisplay()
    }
  }

  @ViewBuilder
  private func cantonesePronunicationDisplay() -> some View {
    switch settings.previewCantonesePhoneticOptions {
    case .rawJyutping:
      if !example.jyutping.isEmpty {
        Text(example.jyutping)
          .fixedSize(horizontal: false, vertical: true)
          .foregroundStyle(.placeholder)
          .textSelection(.enabled)
      }
    case .prettyYale:
      if !example.jyutping.isEmpty {
        Text(
          example.getPhonetic(
            options: .onlyCantonese, cantoneseOptions: .prettyYale,
            mandarinOptions: .prettyPinyin)
        )
        .fixedSize(horizontal: false, vertical: true)
        .foregroundStyle(.placeholder)
        .textSelection(.enabled)
      }
    case .cantoneseIPA:
      if !example.jyutping.isEmpty {
        Text(
          example.getPhonetic(
            options: .onlyCantonese, cantoneseOptions: .cantoneseIPA,
            mandarinOptions: .prettyPinyin)
        )
        .fixedSize(horizontal: false, vertical: true)
        .foregroundStyle(.placeholder)
        .textSelection(.enabled)
      }
    default:
      EmptyView()
    }
  }

  @ViewBuilder
  private func mandarinPronunicationDisplay() -> some View {
    switch settings.previewMandarinPhoneticOptions {
    case .prettyPinyin:
      if !example.pinyin.isEmpty {
        Text(
          example.getPhonetic(
            options: .onlyMandarin, cantoneseOptions: .rawJyutping,
            mandarinOptions: .prettyPinyin)
        )
        .fixedSize(horizontal: false, vertical: true)
        .foregroundStyle(.placeholder)
        .textSelection(.enabled)
      }
    case .numberedPinyin:
      if !example.pinyin.isEmpty {
        Text(example.pinyin)
          .fixedSize(horizontal: false, vertical: true)
          .foregroundStyle(.placeholder)
          .textSelection(.enabled)
      }
    case .zhuyin:
      if !example.pinyin.isEmpty {
        Text(
          example.getPhonetic(
            options: .onlyMandarin, cantoneseOptions: .rawJyutping,
            mandarinOptions: .zhuyin)
        )
        .fixedSize(horizontal: false, vertical: true)
        .foregroundStyle(.placeholder)
        .textSelection(.enabled)
      }
    case .mandarinIPA:
      if !example.pinyin.isEmpty {
        Text(
          example.getPhonetic(
            options: .onlyMandarin, cantoneseOptions: .rawJyutping,
            mandarinOptions: .mandarinIPA)
        )
        .fixedSize(horizontal: false, vertical: true)
        .foregroundStyle(.placeholder)
        .textSelection(.enabled)
      }
    default:
      EmptyView()
    }
  }
}

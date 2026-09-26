//
//  DefinitionCard.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-26.
//

import SwiftUI

struct DefinitionCard: View {
  @Environment(\.colorScheme) var colorScheme

  let set: DefinitionsSet

  var body: some View {
    VStack(alignment: .leading) {
      Text("Definitions (\(set.source))")
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
          set.definitions.enumerated(),
          id: \.element.self
        ) { index, definition in
          HStack(alignment: .top, spacing: 5) {
            Text("\(index + 1)")
              .foregroundStyle(.placeholder)
              .frame(width: 20)

            DefinitionView(definition: definition)
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

private struct DefinitionView: View {
  let definition: Definition

  var body: some View {
    VStack(alignment: .leading) {
      if !definition.label.isEmpty {
        Text(definition.label)
          .fixedSize(horizontal: false, vertical: true)
          .foregroundStyle(.placeholder)
      }
      Text(definition.definitionContent)
        .fixedSize(horizontal: false, vertical: true)
        .textSelection(.enabled)

      ForEach(definition.examples, id: \.id) { example in
        ExampleView(example: example)
      }
    }
  }
}

private struct ExampleView: View {
  let example: Example

  var body: some View {
    HStack(alignment: .top, spacing: 5) {
      Text("◦")
        .foregroundStyle(.placeholder)
        .frame(width: 10, alignment: .leading)
        .padding(.leading, 10)

      VStack(alignment: .leading) {
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
}

//
//  TranslationCard.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-10-29.
//

import SwiftUI

struct TranslationCard: View {
  @Environment(\.colorScheme) var colorScheme

  let set: TranslationSet

  var body: some View {
    VStack(alignment: .leading) {
      Text("Translations (\(set.source))")
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
          set.translations.enumerated(),
          id: \.element.self
        ) { index, translation in
          HStack(alignment: .top, spacing: 5) {
            Text("\(index + 1)")
              .foregroundStyle(.placeholder)
              .frame(width: 20)

            TranslationView(translation: translation)
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

private struct TranslationView: View {
  let translation: Translation

  var body: some View {
    VStack(alignment: .leading) {
      Text(translation.content)
        .fixedSize(horizontal: false, vertical: true)
        .textSelection(.enabled)
    }
  }
}

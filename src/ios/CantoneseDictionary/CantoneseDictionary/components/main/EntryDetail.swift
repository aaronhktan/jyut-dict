//
//  EntryDetail.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-22.
//

import SwiftUI

struct EntryDetail: View {
  @Environment(DatabaseManager.self) private var databaseManager

  let rowId: Int

  @State private var entry: Entry? = nil
  @State private var headerCharacters: AttributedString = ""
  @State private var jyutping: String = ""
  @State private var pinyin: String = ""
  @State private var definitionsSets: [DefinitionsSet] = []

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
            Text(translationSet.getTranslations()[0].content)
              .fixedSize(horizontal: false, vertical: true)
              .textSelection(.enabled)
          }
        }
      }
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

  private struct DefinitionsSetView: View {
    let set: DefinitionsSet

    var body: some View {
      VStack(alignment: .leading) {
        Text(set.source)
          .foregroundStyle(.placeholder)
          .frame(maxWidth: .infinity, alignment: .leading)
          .padding(.horizontal, 12)
          .padding(.vertical, 12)
          .background(
            UnevenRoundedRectangle(topLeadingRadius: 15, topTrailingRadius: 15)
              .fill(.quinary))
          .onTapGesture {
            // Makes deselecting text possible
            UIApplication.shared.sendAction(
              #selector(UIResponder.resignFirstResponder), to: nil, from: nil, for: nil)
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
          .fill(.quinary)
          .onTapGesture {
            // Makes deselecting text possible
            UIApplication.shared.sendAction(
              #selector(UIResponder.resignFirstResponder), to: nil, from: nil, for: nil)
          }
      }
      .padding(.horizontal)
    }
  }

  // Used only for preview
  init(rowId: Int, previewEntry: Entry? = nil) {
    self.rowId = rowId
    _entry = State(initialValue: previewEntry)
    _headerCharacters =
      State(
        initialValue: previewEntry?.getCharacters(
          options: .preferTraditional,
          useColours: true
        ) ?? "Error fetching entry header")
    _jyutping =
      State(
        initialValue: previewEntry?.getPhonetic(
          options: .onlyCantonese,
          cantoneseOptions: .rawJyutping,
          mandarinOptions: .prettyPinyin
        ) ?? "Error fetching Jyutping")
    _pinyin =
      State(
        initialValue: previewEntry?.getPhonetic(
          options: .onlyMandarin,
          cantoneseOptions: .rawJyutping,
          mandarinOptions: .prettyPinyin
        ) ?? "Error fetching Pinyin"
      )

    _definitionsSets = State(initialValue: previewEntry?.getDefinitionsSets() ?? [])
  }

  var body: some View {
    List {
      VStack(alignment: .leading) {
        Text(headerCharacters)
          .font(.largeTitle)
          .frame(maxWidth: .infinity, alignment: .leading)
          .textSelection(.enabled)
        Grid {
          GridRow {
            HStack {
              Text("JP")
                .foregroundStyle(.placeholder)
                .frame(
                  width: 30,
                  alignment: .leading
                )
              Text(jyutping)
                .padding(.trailing)
                .frame(maxWidth: .infinity, alignment: .leading)
                .textSelection(.enabled)
              Spacer()
            }
          }
          GridRow {
            HStack {
              Text("PY")
                .foregroundStyle(.placeholder)
                .frame(width: 30, alignment: .leading)
              Text(pinyin)
                .padding(.trailing)
                .frame(maxWidth: .infinity, alignment: .leading)
                .textSelection(.enabled)
              Spacer()
            }
          }
        }
      }
      .padding(.horizontal)
      .listRowSeparator(.hidden)
      .listRowBackground(Color.clear)
      .listRowInsets(EdgeInsets(top: 0, leading: 0, bottom: 10, trailing: 0))

      ForEach(definitionsSets, id: \.id) { set in
        DefinitionsSetView(set: set)
          .listRowSeparator(.hidden)
          .listRowBackground(Color.clear)
          .listRowInsets(EdgeInsets(top: 5, leading: 0, bottom: 5, trailing: 0))
      }
    }
    .listStyle(.plain)
    .scrollContentBackground(.hidden)
    .task {
      guard entry == nil else { return }
      guard let pool = databaseManager.dbPool else { return }
      let searcher = SQLSearch(pool: pool)

      entry = await searcher.searchByRowId(rowid: String(rowId))
      headerCharacters =
        entry?.getCharacters(
          options: .preferTraditional,
          useColours: true
        ) ?? "Error fetching entry header"
      jyutping =
        entry?.getPhonetic(
          options: .onlyCantonese,
          cantoneseOptions: .rawJyutping,
          mandarinOptions: .prettyPinyin
        ) ?? "Error fetching Jyutping"
      pinyin =
        entry?.getPhonetic(
          options: .onlyMandarin,
          cantoneseOptions: .rawJyutping,
          mandarinOptions: .prettyPinyin
        ) ?? "Error fetching Pinyin"

      definitionsSets = entry?.getDefinitionsSets() ?? []
    }
  }
}

#Preview {
  @Previewable @State var entry = Entry(
    rowid: 0,
    traditional: "聽日",
    simplified: "听日",
    jyutping: "ting1 jat6",
    pinyin: "ting1 ri4",
    definitions: [
      DefinitionsSet(
        source: "CC-CANTO",
        sourceShortString: "CCY",
        definitions: [
          Definition(
            definitionContent: "tomorrow",
            label: "noun",
            examples: [
              Example(
                sourceLanguage: "yue", simplified: "听日", traditional: "聽日", jyutping: "ting1 jat6",
                pinyin: "ting1 ri4",
                translations: [
                  TranslationSet(
                    source: "CCY",
                    translations: [
                      Translation(content: "tomorrow", language: "eng", directTarget: true)
                    ])
                ])
            ]
          )
        ]
      )
    ]
  )
  @Previewable @State var databaseManager = DatabaseManager()

  EntryDetail(rowId: 1, previewEntry: entry)
    .environment(databaseManager)
}

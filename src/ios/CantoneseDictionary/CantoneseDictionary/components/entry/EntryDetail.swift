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
  @State private var examples: [String: [Example]] = [:]
  private var headerCharacters: AttributedString {
    entry?.getCharacters(options: .preferTraditional, useColours: true)
      ?? "Error fetching entry header"
  }
  private var jyutping: String {
    entry?.getPhonetic(
      options: .onlyCantonese, cantoneseOptions: .rawJyutping, mandarinOptions: .prettyPinyin)
      ?? "Error fetching Jyutping"
  }
  private var pinyin: String {
    entry?.getPhonetic(
      options: .onlyMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .prettyPinyin)
      ?? "Error fetching Pinyin"
  }
  private var definitionsSets: [DefinitionsSet] {
    entry?.getDefinitionsSets() ?? []
  }

  // Used only for preview
  init(rowId: Int, previewEntry: Entry? = nil) {
    self.rowId = rowId
    _entry = State(initialValue: previewEntry)
  }

  var body: some View {
    Group {
      if entry != nil {
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
                }
              }
            }
          }
          .padding(.horizontal)
          .listRowSeparator(.hidden)
          .listRowBackground(Color.clear)
          .listRowInsets(EdgeInsets(top: 0, leading: 0, bottom: 10, trailing: 0))

          ForEach(definitionsSets, id: \.id) { set in
            DefinitionCard(set: set)
              .listRowSeparator(.hidden)
              .listRowBackground(Color.clear)
              .listRowInsets(EdgeInsets(top: 5, leading: 0, bottom: 5, trailing: 0))
          }

          ForEach(
            examples.sorted { $0.key > $1.key },
            id: \.key
          ) { source, examples in
            ExampleCard(source: source, examples: examples)
              .listRowSeparator(.hidden)
              .listRowBackground(Color.clear)
              .listRowInsets(EdgeInsets(top: 5, leading: 0, bottom: 5, trailing: 0))
          }
        }
        .listStyle(.plain)
        .scrollContentBackground(.hidden)
      } else {
        Spacer()
      }
    }
    .task(id: rowId) {
      guard rowId != -1 else { return }
      await fetchEntry(rowId: rowId)
    }
  }

  private func fetchEntry(rowId: Int) async {
    guard let pool = databaseManager.dbPool else {
      return
    }
    let searcher = SQLSearch(pool: pool)

    entry = await searcher.searchByRowId(rowid: String(rowId))
    if entry != nil {
      let allExamples = await searcher.searchExamplesByTraditional(searchTerm: entry!.traditional)

      var examplesBySource: [String: [Example]] = [:]
      for example in allExamples {
        if let source = example.getTranslationSets().first?.getSource() {
          if examplesBySource[source, default:[]].count >= 2 {
            // Limit to showing two examples per source for now
            continue
          } else {
            examplesBySource[source, default:[]].append(example)
          }
        }
      }
      examples = examplesBySource
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
                sourceLanguage: "yue",
                traditional: "我唔理，總之我聽朝返到公司要見到你份報告。",
                simplified: "我唔理，总之我听朝返到公司要见到你份报告。",
                jyutping:
                  "ngo5 m4 lei5, zung2 zi1 ngo5 ting1 ziu1 faan1 dou3 gung1 si1 jiu3 gin3 dou2 nei5 fan6 bou3 gou3.",
                pinyin: "ting1 ri4",
                translations: [
                  TranslationSet(
                    source: "CCY",
                    translations: [
                      Translation(
                        content:
                          "I don't give a damn about anything else but your report on my desk tomorrow morning.",
                        language: "eng", directTarget: true)
                    ])
                ])
            ]
          )
        ]
      )
    ]
  )
  @Previewable @State var databaseManager = DatabaseManager()

  EntryDetail(rowId: -1, previewEntry: entry)
    .environment(databaseManager)
}

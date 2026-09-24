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
    @Environment(\.colorScheme) var colorScheme

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
              .fill((colorScheme == .dark) ? AnyShapeStyle(.quinary) : AnyShapeStyle(.gray.opacity(0.1)))
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
          .fill((colorScheme == .dark) ? AnyShapeStyle(.quinary) : AnyShapeStyle(.gray.opacity(0.1)))
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
      } else {
        Spacer()
      }
    }
    .onChange(of: rowId) {
      Task {
        await fetchEntry(rowId: rowId)
      }
    }
    .task {
      guard self.entry == nil else { return }
      await fetchEntry(rowId: rowId)
    }
  }

  private func fetchEntry(rowId: Int) async {
    guard let pool = databaseManager.dbPool else {
      return
    }
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
                sourceLanguage: "yue", simplified: "我唔理，总之我听朝返到公司要见到你份报告。",
                traditional: "我唔理，總之我聽朝返到公司要見到你份報告。",
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

  EntryDetail(rowId: 1, previewEntry: entry)
    .environment(databaseManager)
}

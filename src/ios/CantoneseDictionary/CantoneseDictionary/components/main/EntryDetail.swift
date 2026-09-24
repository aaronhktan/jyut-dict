//
//  EntryDetail.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-22.
//

import SwiftUI

struct EntryDetail: View {
  let entry: Entry

  @State var cantonesePhonetic = ""
  @State var mandarinPhonetic = ""

  var body: some View {
    ScrollView {
      VStack {
        Text(
          entry.getCharacters(
            options: .preferTraditional,
            useColours: true
          )
        )
        .font(.largeTitle)
        .padding(.horizontal)
        .frame(maxWidth: .infinity, alignment: .leading)
        Grid {
          GridRow {
            HStack {
              Text("JP")
                .foregroundStyle(.placeholder)
                .frame(
                  width: 30,
                  alignment: .leading
                )
              Text(
                cantonesePhonetic
              )
              .padding(.trailing)
              .frame(maxWidth: .infinity, alignment: .leading)
              Spacer()
            }
          }
          GridRow {
            HStack {
              Text("PY")
                .foregroundStyle(.placeholder)
                .frame(width: 30, alignment: .leading)
              Text(
                mandarinPhonetic
              )
              .padding(.trailing)
              .frame(maxWidth: .infinity, alignment: .leading)
              Spacer()
            }
          }
        }
        .padding(.horizontal)

        Spacer()
          .padding(.vertical, 3)

        if let definitionsSets = entry.getDefinitionsSets() {
          ForEach(definitionsSets, id: \.self) { set in
            ZStack(alignment: .topLeading) {
              VStack {
                Text(set.source)
                  .foregroundStyle(.placeholder)
                  .frame(
                    maxWidth: .infinity,
                    alignment: .leading
                  )
                  .padding(.horizontal, 12)
                  .padding(.vertical, 12)
                  .background(.quinary)
                VStack {
                  ForEach(
                    set.definitions.enumerated(),
                    id: \.self.element
                  ) { index, definition in
                    HStack(alignment: .top, spacing: 5) {
                      Text("\(index + 1)")
                        .foregroundStyle(.placeholder)
                        .frame(
                          width: 20,
                          alignment: .leading
                        )

                      let definitionText: AttributedString = {
                        if !definition.label.isEmpty {
                          var label =
                            AttributedString(
                              definition.label
                            )
                          label.foregroundColor =
                            Color(
                              uiColor:
                                .placeholderText
                            )
                          return
                            label
                            + AttributedString(
                              "\n"
                            )
                            + AttributedString(
                              definition
                                .definitionContent
                            )
                        } else {
                          return
                            AttributedString(
                              definition
                                .definitionContent
                            )
                        }
                      }()
                      Text(
                        "\(definitionText)"
                      )
                      .frame(
                        maxWidth: .infinity,
                        alignment: .leading
                      )
                    }
                  }
                }
                .padding(.top, 3)
                .padding(.horizontal, 12)
                .padding(.bottom)
              }
              .background(.quaternary)
              .clipShape(RoundedRectangle(cornerRadius: 15))
            }
            .padding(.horizontal)
          }
        }
      }
    }
    .task {
      cantonesePhonetic = await entry.getPhonetic(
        options: .onlyCantonese,
        cantoneseOptions: .rawJyutping,
        mandarinOptions: .prettyPinyin
      )
      mandarinPhonetic = await entry.getPhonetic(
        options: .onlyMandarin,
        cantoneseOptions: .rawJyutping,
        mandarinOptions: .prettyPinyin
      )
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
            examples: []
          )
        ]
      )
    ]
  )
  EntryDetail(entry: entry)
}

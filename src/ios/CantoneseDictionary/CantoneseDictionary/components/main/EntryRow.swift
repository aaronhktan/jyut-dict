//
//  EntryRow.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-22.
//

import SwiftUI

struct EntryRow: View {
  let entry: Entry

  var body: some View {
    VStack {
      Text(
        entry.getCharacters(
          options: .preferTraditional,
          useColours: true
        )
      )
      .font(.title)
      .frame(maxWidth: .infinity, alignment: .leading)
      .lineLimit(1)
      .truncationMode(.tail)
      Text(
        entry.getPhonetic(
          options: .preferCantonese,
          cantoneseOptions: .rawJyutping,
          mandarinOptions: .prettyPinyin
        )
      )
      .frame(maxWidth: .infinity, alignment: .leading)
      .lineLimit(1)
      .truncationMode(.tail)
      Text(
        entry.getDefinitionSnippet()
      )
      .frame(maxWidth: .infinity, alignment: .leading)
      .lineLimit(1)
      .truncationMode(.tail)
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
    definitions: []
  )
  EntryRow(entry: entry)
}

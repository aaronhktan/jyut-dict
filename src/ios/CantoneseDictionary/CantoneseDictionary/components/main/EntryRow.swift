//
//  EntryRow.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-22.
//

import SwiftUI

struct EntryRow: View {
    let entry: Entry
    
    @State private var phonetic: String? = nil

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
                phonetic ?? "placeholder text"
            )
            .frame(maxWidth: .infinity, alignment: .leading)
            .lineLimit(1)
            .truncationMode(.tail)
            .redacted(reason: phonetic == nil ? .placeholder : [])
            Text(
                entry.getDefinitionSnippet()
            )
            .frame(maxWidth: .infinity, alignment: .leading)
            .lineLimit(1)
            .truncationMode(.tail)
        }
        .task {
            phonetic = await entry.getPhonetic(
                options: .preferCantonese,
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
        definitions: []
    )
    EntryRow(entry: entry)
}

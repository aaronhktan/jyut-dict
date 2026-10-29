//
//  EntryRow.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-22.
//

import SwiftUI

struct EntryRow: View {
  @Environment(Settings.self) private var settings

  let entry: Entry

  var body: some View {
    VStack(alignment: .leading) {
      Text(
        entry.getCharacters(
          options: settings.entryCharactersOptions,
          useColours: true,
          colourPhoneticType: settings.entryColourPhoneticType,
          jyutpingToneColours: settings.jyutpingToneColours,
          pinyinToneColours: settings.pinyinToneColours
        )
      )
      .font(.title)
      .rowLine()
      Text(
        // Experimented with making this async, but then it led to undesirable flashes when
        // entries were being updated in the list
        entry.getPhonetic(
          options: settings.previewPhoneticOptions,
          cantoneseOptions: settings.previewCantonesePhoneticOptions,
          mandarinOptions: settings.previewMandarinPhoneticOptions
        )
      )
      .rowLine()
      Text(entry.getDefinitionSnippet())
        .rowLine()
    }
  }
}

#Preview {
  @Previewable @State var settings = Settings()

  let entry = Entry(
    rowid: 0,
    traditional: "聽日",
    simplified: "听日",
    jyutping: "ting1 jat6",
    pinyin: "ting1 ri4",
    definitions: []
  )

  EntryRow(entry: entry)
    .environment(settings)
}

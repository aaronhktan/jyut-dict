//
//  EntryRow.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-22.
//

import SwiftUI

struct EntryRow: View {
  @Environment(DatabaseManager.self) private var databaseManager
  @Environment(Settings.self) private var settings

  @State private var saved = false

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
    .onAppear {
      guard let pool = databaseManager.dbPool else { return }
      let userData = SQLUserData(pool: pool)
      Task {
        saved = await userData.fetchSavedByUnique(
          traditional: entry.traditional, simplified: entry.simplified,
          jyutping: entry.jyutping,
          pinyin: entry.pinyin)
      }
    }
    .swipeActions(edge: .trailing) {
      Button {
        guard let pool = databaseManager.dbPool else { return }
        let userData = SQLUserData(pool: pool)
        Task {
          if saved {
            await userData.removeSaved(
              traditional: entry.traditional, simplified: entry.simplified,
              jyutping: entry.jyutping, pinyin: entry.pinyin)
          } else {
            await userData.addSaved(
              traditional: entry.traditional, simplified: entry.simplified,
              jyutping: entry.jyutping, pinyin: entry.pinyin)
          }
          saved = await userData.fetchSavedByUnique(
            traditional: entry.traditional, simplified: entry.simplified,
            jyutping: entry.jyutping,
            pinyin: entry.pinyin)
        }
      } label: {
        if saved {
          Label("Remove from saved", systemImage: "star.slash")
        } else {
          Label("Save", systemImage: "star.fill")
        }
      }
    }
    .contextMenu {
      let pasteboard = UIPasteboard.general
      Button {
        pasteboard.string = entry.traditional
        Task {
          try? await Task.sleep(for: .seconds(1))
        }
      } label: {
        Text("Copy traditional Chinese")
      }
      Button {
        pasteboard.string = entry.simplified
        Task {
          try? await Task.sleep(for: .seconds(1))
        }
      } label: {
        Text("Copy simplified Chinese")
      }

      Divider()

      Button {
        pasteboard.string = entry.jyutping
        Task {
          try? await Task.sleep(for: .seconds(1))
        }
      } label: {
        Text("Copy Jyutping")
      }
      Button {
        pasteboard.string = entry.getPhonetic(
          options: .onlyCantonese, cantoneseOptions: .prettyYale,
          mandarinOptions: .prettyPinyin)
        Task {
          try? await Task.sleep(for: .seconds(1))
        }
      } label: {
        Text("Copy Yale")
      }
      Button {
        pasteboard.string = entry.getPhonetic(
          options: .onlyCantonese, cantoneseOptions: .cantoneseIPA,
          mandarinOptions: .prettyPinyin)
        Task {
          try? await Task.sleep(for: .seconds(1))
        }
      } label: {
        Text("Copy Cantonese IPA")
      }

      Divider()

      Button {
        pasteboard.string = entry.getPhonetic(
          options: .onlyMandarin, cantoneseOptions: .rawJyutping,
          mandarinOptions: .prettyPinyin)
        Task {
          try? await Task.sleep(for: .seconds(1))
        }
      } label: {
        Text("Copy Pinyin")
      }
      Button {
        pasteboard.string = entry.getPhonetic(
          options: .onlyMandarin, cantoneseOptions: .rawJyutping,
          mandarinOptions: .rawPinyin)
        Task {
          try? await Task.sleep(for: .seconds(1))
        }
      } label: {
        Text("Copy Pinyin with digits")
      }
      Button {
        pasteboard.string = entry.getPhonetic(
          options: .onlyMandarin, cantoneseOptions: .rawJyutping,
          mandarinOptions: .zhuyin)
        Task {
          try? await Task.sleep(for: .seconds(1))
        }
      } label: {
        Text("Copy Zhuyin")
      }
      Button {
        pasteboard.string = entry.getPhonetic(
          options: .onlyMandarin, cantoneseOptions: .rawJyutping,
          mandarinOptions: .mandarinIPA)
        Task {
          try? await Task.sleep(for: .seconds(1))
        }
      } label: {
        Text("Copy Mandarin IPA")
      }
    }
  }

}

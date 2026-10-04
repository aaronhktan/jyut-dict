//
//  SettingsView.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-08.
//

import SwiftUI

struct SettingsView: View {
  @Environment(\.dismiss) var dismiss
  @Environment(Settings.self) private var settings

  var body: some View {
    @Bindable var settings = settings

    NavigationStack {
      Form {
        Section(header: Text("Chinese Characters")) {
          Picker("Character set", selection: $settings.entryCharactersOptions) {
            Text("Only Traditional").tag(EntryCharactersOptions.onlyTraditional)
            Text("Only Simplified").tag(EntryCharactersOptions.onlySimplified)
            Text("Prefer Traditional").tag(EntryCharactersOptions.preferTraditional)
            Text("Prefer Simplified").tag(EntryCharactersOptions.preferSimplified)
          }
          .pickerStyle(.navigationLink)
          Picker("Colour Characters with Tones from", selection: $settings.entryColourPhoneticType)
          {
            Text("Cantonese").tag(EntryColourPhoneticType.cantonese)
            Text("Mandarin").tag(EntryColourPhoneticType.mandarin)
            Text("No Colours").tag(EntryColourPhoneticType.none)
          }
          .pickerStyle(.navigationLink)
        }
        Section(header: Text("In Search Results and Examples")) {
          Picker("Pronunciation Language", selection: $settings.previewPhoneticOptions) {
            Text("Only Cantonese").tag(EntryPhoneticOptions.onlyCantonese)
            Text("Only Mandarin").tag(EntryPhoneticOptions.onlyMandarin)
            Text("Prefer Cantonese").tag(EntryPhoneticOptions.preferCantonese)
            Text("Prefer Mandarin").tag(EntryPhoneticOptions.preferMandarin)
          }
          .pickerStyle(.navigationLink)
          Picker("Cantonese Pronunciation", selection: $settings.previewCantonesePhoneticOptions) {
            Text("Jyutping").tag(CantoneseOptions.rawJyutping)
            Text("Yale").tag(CantoneseOptions.prettyYale)
            Text("Cantonese IPA").tag(CantoneseOptions.cantoneseIPA)
          }
          .pickerStyle(.navigationLink)
          Picker("Mandarin Pronunciation", selection: $settings.previewMandarinPhoneticOptions) {
            Text("Pinyin").tag(MandarinOptions.prettyPinyin)
            Text("Pinyin with Digits").tag(MandarinOptions.rawPinyin)
            Text("Zhuyin").tag(MandarinOptions.zhuyin)
            Text("Mandarin IPA").tag(MandarinOptions.mandarinIPA)
          }
          .pickerStyle(.navigationLink)
        }
        Section(header: Text("When Viewing Entry")) {
          Toggle(
            "Show Jyutping",
            isOn: Binding(
              get: { settings.entryCantonesePhoneticOptions.contains(.rawJyutping) },
              set: { settings.entryCantonesePhoneticOptions.set(.rawJyutping, enabled: $0) }))
          Toggle(
            "Show Yale",
            isOn: Binding(
              get: { settings.entryCantonesePhoneticOptions.contains(.prettyYale) },
              set: { settings.entryCantonesePhoneticOptions.set(.prettyYale, enabled: $0) }))
          Toggle(
            "Show Cantonese IPA",
            isOn: Binding(
              get: { settings.entryCantonesePhoneticOptions.contains(.cantoneseIPA) },
              set: { settings.entryCantonesePhoneticOptions.set(.cantoneseIPA, enabled: $0) }))
          Toggle(
            "Show Pinyin",
            isOn: Binding(
              get: { settings.entryMandarinPhoneticOptions.contains(.rawPinyin) },
              set: { settings.entryMandarinPhoneticOptions.set(.rawPinyin, enabled: $0) }))
          Toggle(
            "Show Pinyin with Digits",
            isOn: Binding(
              get: { settings.entryMandarinPhoneticOptions.contains(.prettyPinyin) },
              set: { settings.entryMandarinPhoneticOptions.set(.prettyPinyin, enabled: $0) }))
          Toggle(
            "Show Zhuyin",
            isOn: Binding(
              get: { settings.entryMandarinPhoneticOptions.contains(.zhuyin) },
              set: { settings.entryMandarinPhoneticOptions.set(.zhuyin, enabled: $0) }))
          Toggle(
            "Show Mandarin IPA",
            isOn: Binding(
              get: { settings.entryMandarinPhoneticOptions.contains(.mandarinIPA) },
              set: { settings.entryMandarinPhoneticOptions.set(.mandarinIPA, enabled: $0) }))
        }
        Section(header: Text("Cantonese Tone Colours")) {
          ColorPicker("Jyutping Tone 1", selection: $settings.jyutpingToneColours[1])
          ColorPicker("Jyutping Tone 2", selection: $settings.jyutpingToneColours[2])
          ColorPicker("Jyutping Tone 3", selection: $settings.jyutpingToneColours[3])
          ColorPicker("Jyutping Tone 4", selection: $settings.jyutpingToneColours[4])
          ColorPicker("Jyutping Tone 5", selection: $settings.jyutpingToneColours[5])
          ColorPicker("Jyutping Tone 6", selection: $settings.jyutpingToneColours[6])
        }
        Section(header: Text("Mandarin Tone Colours")) {
          ColorPicker("Pinyin Tone 1", selection: $settings.pinyinToneColours[1])
          ColorPicker("Pinyin Tone 2", selection: $settings.pinyinToneColours[2])
          ColorPicker("Pinyin Tone 3", selection: $settings.pinyinToneColours[3])
          ColorPicker("Pinyin Tone 4", selection: $settings.pinyinToneColours[4])
          ColorPicker("Pinyin Tone 5", selection: $settings.pinyinToneColours[5])
        }
      }
      .toolbar {
        #if os(iOS)
          ToolbarItem(placement: .topBarTrailing) {
            Button("Close settings", systemImage: "xmark") {
              dismiss()
            }
            .labelsHidden()
          }
        #else
          ToolbarItem(placement: .navigation) {
            Button("Close settings", systemImage: "xmark") {
              dismiss()
            }
            .labelsHidden()
          }
        #endif
      }
      .navigationTitle("Settings")
    }

  }
}

#Preview {
  @Previewable @State var settings = Settings()
  SettingsView()
    .environment(settings)
}

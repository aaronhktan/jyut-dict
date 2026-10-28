//
//  SettingsView.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-08.
//

import SwiftUI
import UniformTypeIdentifiers
import os

struct SettingsView: View {
  @Environment(DatabaseManager.self) private var databaseManager
  @Environment(\.dismiss) var dismiss
  @Environment(Settings.self) private var settings

  @State private var showDatabaseExport = false
  @State private var showDatabaseImport = false
  @State private var showUserDatabaseExport = false
  @State private var showUserDatabaseImport = false

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
              get: { settings.entryMandarinPhoneticOptions.contains(.prettyPinyin) },
              set: { settings.entryMandarinPhoneticOptions.set(.prettyPinyin, enabled: $0) }))
          Toggle(
            "Show Pinyin with Digits",
            isOn: Binding(
              get: { settings.entryMandarinPhoneticOptions.contains(.rawPinyin) },
              set: { settings.entryMandarinPhoneticOptions.set(.rawPinyin, enabled: $0) }))
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
        Section(header: Text("Sources")) {
          NavigationLink {
            SourceManagementView()
          } label: {
            Text("Manage Sources")
          }
        }
        Section(header: Text("Advanced")) {
          List {
            Button("Export dictionary database") {
              Task {
                await databaseManager.checkpointDatabase()
                showDatabaseExport = true
              }
            }
            .tint(.primary)
            .fileExporter(
              isPresented: $showDatabaseExport,
              document: DatabaseDocument(url: databaseManager.dbURL),
              defaultFilename: "dict.db"
            ) { result in
              switch result {
              case .success(let url):
                logger.info("Saved database to \(url)")
              case .failure(let error):
                logger.warning("Export failed: \(error)")
              }
            }
            Button("Import dictionary database") {
              showDatabaseImport = true
            }
            .tint(.primary)
            .fileImporter(
              isPresented: $showDatabaseImport, allowedContentTypes: [.data]
            ) { result in
              switch result {
              case .success(let url):
                Task {
                  guard url.startAccessingSecurityScopedResource() else {
                    return
                  }
                  defer { url.stopAccessingSecurityScopedResource() }
                  await databaseManager.deleteDatabase()
                  await databaseManager.importDatabase(from: url)
                  databaseManager.setupDatabase()
                }
              case .failure(let error):
                logger.error("User database file import failed: \(error)")
              }
            }
            Button("Export search, view, and favourites database") {
              Task {
                await databaseManager.checkpointUserDatabase()
                showUserDatabaseExport = true
              }
            }
            .tint(.primary)
            .fileExporter(
              isPresented: $showUserDatabaseExport,
              document: DatabaseDocument(url: databaseManager.userURL),
              defaultFilename: "user.db"
            ) { result in
              switch result {
              case .success(let url):
                logger.info("Saved user database to \(url)")
              case .failure(let error):
                logger.warning("Export failed: \(error)")
              }
            }
            Button("Import search, view, and favourites database") {
              showUserDatabaseImport = true
            }
            .tint(.primary)
            .fileImporter(
              isPresented: $showUserDatabaseImport, allowedContentTypes: [.data]
            ) { result in
              switch result {
              case .success(let url):
                Task {
                  guard url.startAccessingSecurityScopedResource() else {
                    return
                  }
                  defer { url.stopAccessingSecurityScopedResource() }
                  await databaseManager.deleteUserDatabase()
                  await databaseManager.importUserDatabase(from: url)
                  databaseManager.setupDatabase()
                }
              case .failure(let error):
                logger.error("User database file import failed: \(error)")
              }
            }
          }
        }
        Section {
          Toggle("Enable Unsafe Fuzzy Jyutping", isOn: $settings.unsafeFuzzyJyutping)
        } header: {
          Text("Danger Zone")
        } footer: {
          Text(
            "Several fuzzy Jyutping substitutions are risky. These include (1) ts -> c, and (2) kwu ->(g|k)w?u. Do not enable this setting unless you are confident that you understand and accept the consequences."
          )
        }
        Section(header: Text("More")) {
          NavigationLink {
            AboutView()
          } label: {
            Text("About Jyut Dictionary")
          }
          NavigationLink {
            LicenceView()
          } label: {
            Text("Code Licences")
          }
        }
      }
      .toolbar {
        #if os(iOS)
          ToolbarItem(placement: .topBarLeading) {
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

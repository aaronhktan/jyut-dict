//
//  EntryDetail.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-22.
//

import AVFoundation
import SwiftUI
import os

struct EntryDetail: View {
  @Environment(DatabaseManager.self) private var databaseManager
  @Environment(Settings.self) private var settings

  let rowId: Int

  @State private var entry: Entry? = nil
  @State private var title = ""
  @State private var saved = false

  @State private var animateCantoneseSpeaker = false
  @State private var animateMandarinSpeaker = false
  @State private var speakErrorState: (showAlert: Bool, error: String) = (false, "")
  @State private var player = BufferedSpeechPlayer()

  @State private var allExamples: [Example] = []
  @State private var examplesSnippet: [String: [Example]] = [:]
  @State private var showExampleList = false

  private var definitionsSets: [DefinitionsSet] {
    entry?.getDefinitionsSets() ?? []
  }

  var body: some View {
    Group {
      if entry != nil {
        List {
          VStack(alignment: .leading, spacing: 4) {
            Text(
              entry?.getCharacters(
                options: settings.entryCharactersOptions, useColours: true,
                colourPhoneticType: settings.entryColourPhoneticType,
                jyutpingToneColours: settings.jyutpingToneColours,
                pinyinToneColours: settings.pinyinToneColours
              )
                ?? "Error fetching entry header"
            )
            .font(.largeTitle)
            .frame(maxWidth: .infinity, alignment: .leading)
            .textSelection(.enabled)
            Grid {
              switch settings.previewPhoneticOptions {
              case .onlyCantonese, .preferCantonese:
                cantonesePronunciationView()
                mandarinPronunicationView()
              case .onlyMandarin, .preferMandarin:
                mandarinPronunicationView()
                cantonesePronunciationView()
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
            examplesSnippet.sorted { $0.key > $1.key },
            id: \.key
          ) { source, examples in
            ExampleCard(source: source, examples: examples)
              .listRowSeparator(.hidden)
              .listRowBackground(Color.clear)
              .listRowInsets(EdgeInsets(top: 5, leading: 0, bottom: 5, trailing: 0))
          }

          if !allExamples.isEmpty {
            Button {
              showExampleList = true
            } label: {
              Text("View all examples")
                .frame(maxWidth: .infinity)
            }
            .buttonStyle(.glass)
            .tint(.accent)
            .buttonBorderShape(.capsule)
            .controlSize(.extraLarge)
            .listRowSeparator(.hidden)
            .listRowInsets(EdgeInsets(top: 5, leading: 0, bottom: 10, trailing: 0))
            .padding(.horizontal)
          }
        }
        .navigationDestination(isPresented: $showExampleList) {
          ExampleList(examples: allExamples)
        }
        .listStyle(.plain)
        .scrollContentBackground(.hidden)
      } else {
        Spacer()
      }
    }
    .navigationTitle(title)
    .navigationBarTitleDisplayMode(.inline)
    .toolbar {
      // This is used to hide the navigation title
      ToolbarItem(placement: .principal) {
        Color.clear
          .frame(width: 0, height: 0)
      }
      toolbarContent
    }
    .alert(isPresented: $speakErrorState.showAlert) {
      Alert(
        title: Text("Couldn't speak entry!"),
        message: Text(speakErrorState.error),
        dismissButton: .default(Text("OK")))
    }
    .task(id: rowId) {
      guard rowId != -1 else { return }
      await fetchEntry(rowId: rowId)

      try? await Task.sleep(for: .milliseconds(200))
      guard !Task.isCancelled else { return }

      if let entry {
        guard let pool = databaseManager.dbPool else { return }
        let userHistory = SQLUserHistory(pool: pool)
        await userHistory.addViewToHistory(entry: entry)

        let userData = SQLUserData(pool: pool)
        saved = await userData.fetchSavedByUnique(
          traditional: entry.traditional, simplified: entry.simplified, jyutping: entry.jyutping,
          pinyin: entry.pinyin)

        title = String(
          entry.getCharacters(
            options: settings.entryCharactersOptions, useColours: false,
            colourPhoneticType: settings.entryColourPhoneticType,
            jyutpingToneColours: settings.jyutpingToneColours,
            pinyinToneColours: settings.pinyinToneColours
          ).characters[...])
      }
    }
  }

  private func fetchEntry(rowId: Int) async {
    guard let pool = databaseManager.dbPool else {
      return
    }
    let searcher = SQLSearch(pool: pool)

    entry = await searcher.searchByRowId(rowid: String(rowId))

    examplesSnippet = [:]
    if entry != nil {
      allExamples = await searcher.searchExamplesByTraditional(searchTerm: entry!.traditional)

      var examplesBySource: [String: [Example]] = [:]
      for example in allExamples {
        if let source = example.getTranslationSets().first?.source {
          if examplesBySource[source, default: []].count >= 2 {
            // Limit to showing two examples per source for now
            continue
          } else {
            examplesBySource[source, default: []].append(example)
          }
        }
      }
      examplesSnippet = examplesBySource
    }
  }
}

extension EntryDetail {
  @ViewBuilder
  private func cantonesePronunciationView() -> some View {
    if settings.entryCantonesePhoneticOptions.contains(.rawJyutping) {
      GridRow(alignment: .firstTextBaseline) {
        cantonesePronunciationButton()

        HStack(alignment: .firstTextBaseline) {
          Text("Jyutping")
            .foregroundStyle(.placeholder)
          Text(
            entry?.getPhonetic(
              options: .onlyCantonese, cantoneseOptions: .rawJyutping,
              mandarinOptions: .prettyPinyin)
              ?? "Error fetching Jyutping"
          )
          .padding(.trailing)
          .frame(maxWidth: .infinity, alignment: .leading)
          .fixedSize(horizontal: false, vertical: true)
          .textSelection(.enabled)
        }
      }
    }
    if settings.entryCantonesePhoneticOptions.contains(.prettyYale) {
      GridRow(alignment: .firstTextBaseline) {
        if !settings.entryCantonesePhoneticOptions.contains(.rawJyutping) {
          cantonesePronunciationButton()
        } else {
          Spacer()
        }

        HStack(alignment: .firstTextBaseline) {
          Text("Yale")
            .foregroundStyle(.placeholder)
          Text(
            entry?.getPhonetic(
              options: .onlyCantonese, cantoneseOptions: .prettyYale,
              mandarinOptions: .prettyPinyin)
              ?? "Error fetching Yale"
          )
          .padding(.trailing)
          .frame(maxWidth: .infinity, alignment: .leading)
          .fixedSize(horizontal: false, vertical: true)
          .textSelection(.enabled)

        }
      }
    }
    if settings.entryCantonesePhoneticOptions.contains(.cantoneseIPA) {
      GridRow(alignment: .firstTextBaseline) {
        if !settings.entryCantonesePhoneticOptions.contains(.rawJyutping),
          !settings.entryCantonesePhoneticOptions.contains(.prettyYale)
        {
          cantonesePronunciationButton()
        } else {
          Spacer()
        }

        HStack(alignment: .firstTextBaseline) {
          Text("Cantonese IPA")
            .foregroundStyle(.placeholder)
          Text(
            entry?.getPhonetic(
              options: .onlyCantonese, cantoneseOptions: .cantoneseIPA,
              mandarinOptions: .prettyPinyin)
              ?? "Error fetching Cantonese IPA"
          )
          .padding(.trailing)
          .frame(maxWidth: .infinity, alignment: .leading)
          .fixedSize(horizontal: false, vertical: true)
          .textSelection(.enabled)
        }
      }
    }
  }

  @ViewBuilder
  private func cantonesePronunciationButton() -> some View {
    Button {
      animateCantoneseSpeaker.toggle()

      var words =
        entry?.getPhonetic(
          options: .onlyCantonese, cantoneseOptions: .rawJyutping,
          mandarinOptions: .prettyPinyin) ?? entry?.traditional
      if let w = words, w.isEmpty {
        words = entry?.traditional
      }
      guard let words else {
        speakErrorState = (true, "No words were found to pronounce!")
        if let entry {
          logger.error("Nothing could be pronounced in Cantonese for entry \(entry.traditional)")
        }
        return
      }
      let utterance = AVSpeechUtterance(string: words)

      var voice = AVSpeechSynthesisVoice(identifier: "com.apple.voice.compact.zh-HK.Sinji")
      if voice == nil {
        for v in AVSpeechSynthesisVoice.speechVoices() {
          if v.language == "yue-HK" {
            voice = v
            break
          }
        }
      }
      guard let voice else {
        speakErrorState = (true, "Couldn't load a Cantonese voice!")
        logger.error("No Cantonese voice could be found!")
        return
      }
      utterance.voice = voice

      Task {
        await player.setupAudioSession()
        await player.speak(utterance)
      }
    } label: {
      Label("Pronounce in Cantonese", systemImage: "speaker.wave.2")
        .symbolEffect(.variableColor.iterative, value: animateCantoneseSpeaker)
    }
    .labelStyle(.iconOnly)
    .buttonBorderShape(.circle)
    .buttonStyle(.glass)
    .controlSize(.mini)
  }

  @ViewBuilder
  private func mandarinPronunicationView() -> some View {
    if settings.entryMandarinPhoneticOptions.contains(.prettyPinyin) {
      GridRow(alignment: .firstTextBaseline) {
        mandarinPronunciationButton()

        HStack(alignment: .firstTextBaseline) {
          Text("Pinyin")
            .foregroundStyle(.placeholder)
          Text(
            entry?.getPhonetic(
              options: .onlyMandarin, cantoneseOptions: .rawJyutping,
              mandarinOptions: .prettyPinyin)
              ?? "Error fetching Pinyin"
          )
          .padding(.trailing)
          .frame(maxWidth: .infinity, alignment: .leading)
          .fixedSize(horizontal: false, vertical: true)
          .textSelection(.enabled)
        }
      }
    }
    if settings.entryMandarinPhoneticOptions.contains(.rawPinyin) {
      GridRow(alignment: .firstTextBaseline) {
        if !settings.entryMandarinPhoneticOptions.contains(.prettyPinyin) {
          mandarinPronunciationButton()
        } else {
          Spacer()
        }

        HStack(alignment: .firstTextBaseline) {
          Text("Raw Pinyin")
            .foregroundStyle(.placeholder)
          Text(
            entry?.getPhonetic(
              options: .onlyMandarin, cantoneseOptions: .rawJyutping,
              mandarinOptions: .rawPinyin)
              ?? "Error fetching Pinyin with digits"
          )
          .padding(.trailing)
          .frame(maxWidth: .infinity, alignment: .leading)
          .fixedSize(horizontal: false, vertical: true)
          .textSelection(.enabled)
        }
      }
    }
    if settings.entryMandarinPhoneticOptions.contains(.zhuyin) {
      GridRow(alignment: .firstTextBaseline) {
        if !settings.entryMandarinPhoneticOptions.contains(.prettyPinyin),
          settings.entryMandarinPhoneticOptions.contains(.rawPinyin)
        {
          mandarinPronunciationButton()
        } else {
          Spacer()
        }

        HStack(alignment: .firstTextBaseline) {
          Text("Zhuyin")
            .foregroundStyle(.placeholder)
          Text(
            entry?.getPhonetic(
              options: .onlyMandarin, cantoneseOptions: .rawJyutping,
              mandarinOptions: .zhuyin)
              ?? "Error fetching Zhuyin"
          )
          .padding(.trailing)
          .frame(maxWidth: .infinity, alignment: .leading)
          .fixedSize(horizontal: false, vertical: true)
          .textSelection(.enabled)
        }
      }
    }
    if settings.entryMandarinPhoneticOptions.contains(.mandarinIPA) {
      GridRow(alignment: .firstTextBaseline) {
        if !settings.entryMandarinPhoneticOptions.contains(.prettyPinyin),
          settings.entryMandarinPhoneticOptions.contains(.rawPinyin),
          settings.entryMandarinPhoneticOptions.contains(.zhuyin)
        {
          mandarinPronunciationButton()
        } else {
          Spacer()
        }

        HStack(alignment: .firstTextBaseline) {
          Text("Mandarin IPA")
            .foregroundStyle(.placeholder)
          Text(
            entry?.getPhonetic(
              options: .onlyMandarin, cantoneseOptions: .rawJyutping,
              mandarinOptions: .mandarinIPA)
              ?? "Error fetching Mandarin IPA"
          )
          .padding(.trailing)
          .frame(maxWidth: .infinity, alignment: .leading)
          .fixedSize(horizontal: false, vertical: true)
          .textSelection(.enabled)
        }
      }
    }
  }

  @ViewBuilder
  private func mandarinPronunciationButton() -> some View {
    Button {
      animateMandarinSpeaker.toggle()

      var words =
        entry?.getPhonetic(
          options: .onlyMandarin, cantoneseOptions: .rawJyutping,
          mandarinOptions: .rawPinyin) ?? entry?.traditional
      if let w = words, w.isEmpty {
        words = entry?.traditional
      }
      guard let words else {
        speakErrorState = (true, "No words were found to pronounce!")
        if let entry {
          logger.error("Nothing could be pronounced in Mandarin for entry \(entry.traditional)")
        }
        return
      }
      let utterance = AVSpeechUtterance(string: words)

      var voice = AVSpeechSynthesisVoice(identifier: "com.apple.voice.compact.zh-TW.Meijia")
      if voice == nil {
        for v in AVSpeechSynthesisVoice.speechVoices() {
          if v.language == "zh-TW" {
            voice = v
            break
          }
        }
      }
      if voice == nil {
        for v in AVSpeechSynthesisVoice.speechVoices() {
          if v.language == "zh-CN" {
            voice = v
            break
          }
        }
      }
      guard let voice else {
        speakErrorState = (true, "Couldn't load a Mandarin voice!")
        logger.error("No Mandarin voice could be found!")
        return
      }
      utterance.voice = voice

      Task {
        await player.setupAudioSession()
        await player.speak(utterance)
      }
    } label: {
      Label("Pronounce in Mandarin", systemImage: "speaker.wave.2")
        .symbolEffect(.variableColor.iterative, value: animateMandarinSpeaker)
    }
    .labelStyle(.iconOnly)
    .buttonBorderShape(.circle)
    .buttonStyle(.glass)
    .controlSize(.mini)
  }

  // MARK: - Toolbar
  private var topPlacement: ToolbarItemPlacement {
    #if os(iOS)
      .topBarTrailing
    #else
      .navigation
    #endif
  }

  @ToolbarContentBuilder
  private var toolbarContent: some ToolbarContent {
    ToolbarItem(placement: topPlacement) {
      Button {
        guard let pool = databaseManager.dbPool else { return }

        if let entry {
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
        }
      } label: {
        Label("Save entry", systemImage: saved ? "star.fill" : "star")
      }
      .labelsHidden()
      .glassEffect()
    }
  }
}

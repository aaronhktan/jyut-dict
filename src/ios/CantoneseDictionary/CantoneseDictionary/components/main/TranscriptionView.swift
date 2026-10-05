//
//  TranscriptionView.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-04.
//

import Speech
import SwiftUI
import os

struct TranscriptionView: View {
  @Environment(\.dismiss) var dismiss

  @Bindable var searchContext: SearchContext

  @Binding var isSearchActive: Bool

  @State private var transcriber: Transcriber
  @State private var recorder: Recorder
  @State private var languageSelection: DictationLanguage = .cantonese
  @State private var showTranscriptionFailure = false
  @State private var buttonText = "Search"

  private enum DictationLanguage {
    case cantonese
    case mandarin
    case english
    case french

    var descriptionString: String {
      switch self {
      case .cantonese:
        return "Cantonese"
      case .mandarin:
        return "Mandarin"
      case .english:
        return "English"
      case .french:
        return "French"
      }
    }

    var locale: [Locale] {
      switch self {
      case .cantonese:
        return [
          Locale(components: .init(languageCode: .cantonese, script: nil, languageRegion: nil)),
          Locale(
            components: .init(languageCode: .cantonese, script: nil, languageRegion: .hongKong)),
          Locale(
            components: .init(languageCode: .cantonese, script: nil, languageRegion: .chinaMainland),
          ),
        ]
      case .mandarin:
        return [
          Locale(components: .init(languageCode: .chinese, script: nil, languageRegion: nil)),
          Locale(components: .init(languageCode: .chinese, script: nil, languageRegion: .taiwan)),
          Locale(
            components: .init(languageCode: .chinese, script: nil, languageRegion: .chinaMainland)),
        ]
      case .english:
        return [
          Locale(components: .init(languageCode: .english, script: nil, languageRegion: .canada)),
          Locale(
            components: .init(languageCode: .english, script: nil, languageRegion: .unitedKingdom)),
          Locale(
            components: .init(languageCode: .english, script: nil, languageRegion: .unitedStates)),
          Locale(
            components: .init(languageCode: .english, script: nil, languageRegion: .australia)),
          Locale(
            components: .init(languageCode: .english, script: nil, languageRegion: .newZealand)),
          Locale(components: .init(languageCode: .english, script: nil, languageRegion: .india)),
          Locale(components: .init(languageCode: .english, script: nil, languageRegion: .nigeria)),
        ]
      case .french:
        return [
          Locale(components: .init(languageCode: .french, script: nil, languageRegion: .canada)),
          Locale(components: .init(languageCode: .french, script: nil, languageRegion: .france)),
          Locale(components: .init(languageCode: .french, script: nil, languageRegion: .belgium)),
          Locale(
            components: .init(languageCode: .french, script: nil, languageRegion: .switzerland)),
          Locale(components: .init(languageCode: .french, script: nil, languageRegion: .algeria)),
        ]
      }
    }
  }

  init(searchContext: SearchContext, isSearchActive: Binding<Bool>) {
    self.searchContext = searchContext
    self._isSearchActive = isSearchActive
    let transcriber = Transcriber()
    recorder = Recorder(transcriber: transcriber)
    self.transcriber = transcriber
  }

  var body: some View {
    NavigationStack {
      VStack {
        ZStack {
          Circle()
            .fill(.background)
            .frame(width: min(CGFloat(transcriber.audioLevel * 150 + 75), 150))
            .animation(.easeInOut(duration: 0.10), value: transcriber.audioLevel)
          Image(systemName: "microphone.fill")
            .resizable()
            .scaledToFit()
            .frame(width: 30)
            .foregroundStyle(transcriber.isTranscribing ? .accent : .secondary)
            .padding()
        }
        .frame(height: 150)
        .onTapGesture {
          if !transcriber.isTranscribing {
            Task {
              do {
                try await recorder.start(locales: languageSelection.locale)
              } catch {
                logger.error("\(error)")
                showTranscriptionFailure = true
              }
            }
          }
        }
        Spacer()
        Button {
          if !transcriber.transcript.isEmpty {
            searchContext.searchText = transcriber.transcript
            if (languageSelection == .english || languageSelection == .french) {
              searchContext.selectedOption = .english
            } else {
              searchContext.selectedOption = .autoDetect
            }
            isSearchActive = true
          }
          dismiss()
        } label: {
          Text(buttonText)
            .lineLimit(1)
            .truncationMode(.middle)
            .frame(maxWidth: .infinity)
            .contentTransition(.opacity)
            .animation(.easeInOut(duration: 0.3), value: buttonText)
        }
        .buttonStyle(.borderedProminent)
        .buttonBorderShape(.capsule)
        .controlSize(.extraLarge)
        .padding(.horizontal)
      }
      .toolbar {
        #if os(iOS)
          ToolbarItem(placement: .principal) {
            Menu {
              Picker("Language", selection: $languageSelection) {
                Text("Cantonese").tag(DictationLanguage.cantonese)
                Text("Mandarin").tag(DictationLanguage.mandarin)
                Text("English").tag(DictationLanguage.english)
              }
            } label: {
              HStack(spacing: 4) {
                Text(languageSelection.descriptionString)
                  .font(.title3)
                Image(systemName: "chevron.up.chevron.down")
                  .imageScale(.small)
                  .foregroundStyle(.accent)
              }
            }
            .tint(.primary)
          }
          ToolbarItem(placement: .topBarTrailing) {
            Button("Close transcription", systemImage: "xmark") {
              dismiss()
            }
            .labelsHidden()
          }
        #else
          ToolbarItem(placement: .navigation) {
            Button("Close transcription", systemImage: "xmark") {
              dismiss()
            }
            .labelsHidden()
          }
        #endif
      }
    }
    .onChange(of: languageSelection) {
      Task {
        do {
          buttonText = "Search"
          try await recorder.stop()
          try await recorder.start(locales: languageSelection.locale)
        } catch {
          logger.error("\(error)")
          showTranscriptionFailure = true
        }
      }
    }
    .onChange(of: transcriber.transcript) {
      if transcriber.transcript.isEmpty {
        buttonText = "Search"
      } else {
        buttonText = "Search for: \(transcriber.transcript)"
      }
    }
    .task {
      guard await AVCaptureDevice.requestAccess(for: .audio) else {
        showTranscriptionFailure = true
        return
      }
      do {
        try await recorder.start(locales: languageSelection.locale)
      } catch {
        logger.error("\(error)")
        showTranscriptionFailure = true
      }
    }
    .onDisappear {
      Task {
        try? await recorder.stop()
      }
    }
    .alert(
      "Jyut Dictionary was unable to access your microphone.",
      isPresented: $showTranscriptionFailure
    ) {
      Button("OK", role: .cancel) {
        dismiss()
      }
    }
  }
}

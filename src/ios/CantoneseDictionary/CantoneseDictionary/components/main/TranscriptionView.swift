//
//  TranscriptionView.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-04.
//

import SwiftUI

struct TranscriptionView: View {
  @Environment(\.dismiss) var dismiss

  @State private var transcriber = Transcriber()
  @State private var languageSelection = "yue"
  @State private var showTranscriptionFailure = false

  private var displayName: String {
    switch languageSelection {
    case "yue": "Cantonese"
    case "cmn": "Mandarin"
    case "eng": "English"
    case "fra": "French"
    default: "Language"
    }
  }

  var body: some View {
    NavigationStack {
      VStack {
        ZStack {
          Circle()
            .fill(.background)
            .frame(width: CGFloat((transcriber.audioLevel + 40.0) / 20 * 100) + 100)
            .foregroundColor(.blue)
            .animation(.easeOut(duration: 0.15), value: transcriber.audioLevel)
          Image(systemName: "microphone.fill")
            .resizable()
            .scaledToFit()
            .frame(width: 30)
            .foregroundStyle(.accent)
            .padding()
        }
        .frame(height: 250)
        Spacer()
        Button("Search") {
        }
        .buttonStyle(.borderedProminent)
        .buttonBorderShape(.capsule)
        .controlSize(.extraLarge)
      }
      .toolbar {
        #if os(iOS)
          ToolbarItem(placement: .principal) {
            Menu {
              Picker("Language", selection: $languageSelection) {
                Text("Cantonese").tag("yue")
                Text("Mandarin").tag("cmn")
                Text("English").tag("eng")
              }
            } label: {
              HStack(spacing: 4) {
                Text(displayName)
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
    .task {
      let result = await transcriber.start()
      if !result {
        showTranscriptionFailure = true
      }
    }
    .onDisappear {
      transcriber.stop()
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

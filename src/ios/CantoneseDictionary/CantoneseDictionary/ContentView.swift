//
//  ContentView.swift
//  Test
//
//  Created by Aaron on 2026-09-01.
//

import SwiftUI

struct ContentView: View {
    enum Sheet: String, Identifiable {
        case transcription, handwriting, saved, history, settings

        var id: String { rawValue }
    }

    @State var presentedSheet: Sheet?
    @State private var searchText: String = ""
    @State private var searchIsActive = false
    @State private var processedSearchText: [String] = []

    var body: some View {
        NavigationSplitView {
            SearchingView(
                searchText: $searchText,
                isSearchActive: $searchIsActive,
                processedSearchText: $processedSearchText
            )
            .toolbar {
                if !searchIsActive {
                    ToolbarItem(placement: .topBarTrailing) {
                        ControlGroup {
                            Button(action: {
                                presentedSheet = .history
                            }) {
                                Image(systemName: "clock")
                            }
                            Button(action: {
                                presentedSheet = .saved
                            }) {
                                Image(systemName: "star")
                            }
                            Button(action: {
                                presentedSheet = .settings
                            }) {
                                Image(systemName: "gearshape")
                            }
                        }
                    }
                }
                DefaultToolbarItem(kind: .search, placement: .bottomBar)
                ToolbarSpacer(.fixed, placement: .bottomBar)
                ToolbarItem(placement: .bottomBar) {
                    ControlGroup {
                        Button(action: {
                            presentedSheet = .transcription
                        }) {
                            Image(systemName: "microphone")
                        }

                        Button(action: {
                            presentedSheet = .handwriting
                        }) {
                            Image(systemName: "pencil.and.scribble")
                        }
                    }
                }
            }
            .sheet(
                item: $presentedSheet,
                content: { sheet in
                    switch sheet {
                    case .transcription:
                        TranscriptionView()
                            .presentationDetents([.medium])
                    case .handwriting:
                        TranscriptionView()
                            .presentationDetents([.medium])
                    case .saved:
                        SavedView()
                            .presentationDetents([.large])
                    case .history:
                        HistoryView()
                            .presentationDetents([.large])
                    case .settings:
                        SettingsView()
                            .presentationDetents([.large])
                    }
                }
            )
            .searchable(
                text: $searchText,
                isPresented: $searchIsActive,
                prompt: "Search",
            )
            .searchPresentationToolbarBehavior(.avoidHidingContent)
            .onChange(of: searchText) {
                let intermediate = jyutpingAutocorrect(
                    text: searchText,
                    unsafeSubstitutions: true
                )
                let (_, segmented) =
                    segmentJyutping(
                        text: intermediate,
                        removeSpecialCharacters: true,
                        removeGlobCharacters: false,
                        removeRegexCharacters: false
                    )
                let result = jyutpingSoundChanges(text: segmented)
                processedSearchText = result
            }
        } detail: {
            Text("Hi")
        }
    }
}

struct SearchingView: View {
    @Binding var searchText: String
    @Binding var isSearchActive: Bool
    @Binding var processedSearchText: [String]

    @State private var options: [SearchParameters: String] = [
        .autoDetect: "Auto-detect language",
        .traditional: "Traditional Chinese",
        .simplified: "Simplified Chinese",
        .fuzzyJyutping: "Fuzzy Jyutping",
        .jyutping: "Jyutping",
        .fuzzyPinyin: "Fuzzy Pinyin",
        .pinyin: "Pinyin",
        .english: "English",
    ]
    @State private var selectedOption: SearchParameters = .autoDetect
    @State private var isPickerTextVisible = true
    @State private var animationToken: Int = 0

    var body: some View {
        Group {
            if isSearchActive {
                ZStack(alignment: .bottom) {
                    List(processedSearchText, id: \.self) { word in
                        NavigationLink(word, value: word)
                    }
                    .navigationDestination(
                        for: String.self,
                        destination: EntryView.init
                    )
                    .listStyle(.automatic)

                    Picker("Search Options", selection: $selectedOption) {
                        ForEach(
                            options.sorted(by: {
                                $0.key.rawValue < $1.key.rawValue
                            }),
                            id: \.key
                        ) { option, label in
                            Text(label).tag(option)
                        }
                    }
                    .opacity(isPickerTextVisible ? 1 : 0)
                    .glassEffect()
                    .onChange(of: searchText) {
                        animationToken += 1
                        let currentToken = animationToken
                        isPickerTextVisible = false
                        withAnimation(.snappy(duration: 0.1)) {
                            switch searchText.count % 7 {
                            case 0:
                                options[.autoDetect] = "Detected input: English"
                            case 1:
                                options[.autoDetect] =
                                    "Detected input: Simplified Chinese"
                            case 2:
                                options[.autoDetect] =
                                    "Detected input: Traditional Chinese"
                            case 3:
                                options[.autoDetect] =
                                    "Detected input: Fuzzy Jyutping"
                            case 4:
                                options[.autoDetect] =
                                    "Detected input: Jyutping"
                            case 5:
                                options[.autoDetect] =
                                    "Detected input: Fuzzy Pinyin"
                            case 6:
                                options[.autoDetect] = "Detected input: Pinyin"
                            default:
                                options[.autoDetect] = "???"
                            }
                        } completion: {
                            guard animationToken == currentToken else { return }
                            withAnimation(.easeIn(duration: 0.05)) {
                                isPickerTextVisible = true
                            }
                        }
                    }
                }
            } else {
                ContentUnavailableView {
                    Label {
                        Text("Welcome to Jyut Dictionary")
                    } icon: {
                        Image("AppIconImage")
                            .resizable()
                            .scaledToFit()
                            .frame(width: 200)
                    }
                    Text("A Project by Aaron Tan")
                        .padding(.top, 10)
                        .foregroundStyle(Color.secondary)
                }
            }
        }
        .toolbar(isSearchActive ? .hidden : .visible, for: .navigationBar)
        .onChange(of: isSearchActive) {
            if !isSearchActive {
                options[.autoDetect] = "Auto-detect language"
            }
        }
    }
}

#Preview {
    ContentView()
}

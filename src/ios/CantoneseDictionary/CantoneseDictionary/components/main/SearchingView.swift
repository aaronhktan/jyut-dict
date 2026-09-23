//
//  SearchingView.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-22.
//

import SwiftUI

struct SearchingView: View {
    @Environment(DatabaseManager.self) private var databaseManager

    @Binding var searchText: String
    @Binding var isSearchActive: Bool
    @Binding var searchResults: [Entry]

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
    @State private var showEmptyState = false

    struct SearchQuery: Equatable {
        let text: String
        let option: SearchParameters
    }

    var body: some View {
        Group {
            if isSearchActive {
                ZStack(alignment: .bottom) {
                    List(searchResults) { entry in
                        NavigationLink(value: entry) {
                            EntryRow(entry: entry)
                        }
                    }
                    .navigationDestination(
                        for: Entry.self,
                    ) { entry in
                        EntryDetail(entry: entry)
                    }
                    .listStyle(.automatic)
                    .scrollDismissesKeyboard(.immediately)
                    .overlay {
                        if showEmptyState {
                            ContentUnavailableView {
                                Text("No search results were found.")
                            }
                        }
                    }
                    .safeAreaInset(edge: .bottom) {
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
                            guard selectedOption == .autoDetect else { return }
                            animationToken += 1
                            let currentToken = animationToken
                            isPickerTextVisible = false
                            withAnimation(.snappy(duration: 0.1)) {
                                if searchText.isEmpty {
                                    options[.autoDetect] =
                                        "Auto-detect language"
                                    return
                                }
                                switch searchText.count % 7 {
                                case 0:
                                    options[.autoDetect] =
                                        "Detected input: English"
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
                                    options[.autoDetect] =
                                        "Detected input: Pinyin"
                                default:
                                    options[.autoDetect] = "???"
                                }
                            } completion: {
                                guard animationToken == currentToken else {
                                    return
                                }
                                withAnimation(.easeIn(duration: 0.05)) {
                                    isPickerTextVisible = true
                                }
                            }
                        }
                    }
                }
                .task(id: SearchQuery(text: searchText, option: selectedOption))
                {
                    showEmptyState = false
                    guard !searchText.isEmpty else {
                        searchResults = []
                        return
                    }

                    let results = await triggerSearch()
                    guard !Task.isCancelled else { return }
                    searchResults = results

                    guard !results.isEmpty else {
                        try? await Task.sleep(for: .milliseconds(500))
                        guard !Task.isCancelled else { return }
                        showEmptyState = searchResults.isEmpty
                        return
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
                searchResults = []
            }
        }
    }

    private func triggerSearch() async -> [Entry] {
        guard let pool = databaseManager.dbPool else { return [] }
        let searcher = SQLSearch(pool: pool)
        var results: [Entry] = []
        switch selectedOption {
        case .traditional:
            results = await searcher.searchTraditional(
                searchTerm: searchText,
            )
        case .simplified:
            results = await searcher.searchSimplified(
                searchTerm: searchText,
            )
        case .pinyin:
            results = await searcher.searchPinyin(
                searchTerm: searchText.lowercased(),
                useFuzzyPinyin: false
            )
        case .fuzzyPinyin:
            results = await searcher.searchPinyin(
                searchTerm: searchText.lowercased(),
                useFuzzyPinyin: true
            )
        case .jyutping:
            results = await searcher.searchJyutping(
                searchTerm: searchText.lowercased(),
                useFuzzyJyutping: false
            )
        case .fuzzyJyutping:
            fallthrough
        default:
            results = await searcher.searchJyutping(
                searchTerm: searchText.lowercased(),
                useFuzzyJyutping: true
            )
        }

        return results
    }
}

#Preview {
    @Previewable @State var databaseManager = DatabaseManager()

    @Previewable @State var searchText: String = ""
    @Previewable @State var isSearchActive: Bool = true
    @Previewable @State var searchResults: [Entry] = []
    SearchingView(
        searchText: $searchText,
        isSearchActive: $isSearchActive,
        searchResults: $searchResults
    )
    .environment(databaseManager)
}

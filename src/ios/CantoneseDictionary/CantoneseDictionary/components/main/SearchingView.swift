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

  @State private var options: [InputMethod: String] = [
    .autoDetect: "Auto-detect language",
    .traditional: "Traditional Chinese",
    .simplified: "Simplified Chinese",
    .fuzzyJyutping: "Fuzzy Jyutping",
    .jyutping: "Jyutping",
    .fuzzyPinyin: "Fuzzy Pinyin",
    .pinyin: "Pinyin",
    .english: "English",
  ]
  @State private var selectedOption: InputMethod = .autoDetect
  @State private var detectedInputMethod: InputMethod = .none
  @State private var isPickerTextVisible = true
  @State private var animationToken: Int = 0
  @State private var showEmptyState = false

  private var isIPad: Bool {
    UIDevice.current.userInterfaceIdiom == .pad
  }
  private var shouldHideToolbar: Bool {
    // Would be better to use HorizontalSizeClass, but causes bugs on iPad
    // right now :(
    !isIPad && isSearchActive
  }

  private struct SearchQuery: Equatable {
    let text: String
    let option: InputMethod
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
            .padding(.bottom, isIPad ? 10 : 0)
            .onChange(of: detectedInputMethod) { oldMethod, newMethod in
              guard selectedOption == .autoDetect else { return }
              if !searchText.isEmpty && newMethod == oldMethod {
                return
              }
              animationToken += 1
              let currentToken = animationToken
              isPickerTextVisible = false
              withAnimation(.snappy(duration: 0.1)) {
                if searchText.isEmpty {
                  options[.autoDetect] =
                    "Auto-detect language"
                  return
                }
                options[.autoDetect] =
                  "Detected input: \(InputMethodNames[detectedInputMethod]!)"
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
        .task(id: SearchQuery(text: searchText, option: selectedOption)) {
          showEmptyState = false
          guard !searchText.isEmpty else {
            searchResults = []
            detectedInputMethod = .autoDetect
            return
          }

          let results = await triggerSearch()
          guard !Task.isCancelled else { return }

          if results.isEmpty {
            try? await Task.sleep(for: .milliseconds(500))
            guard !Task.isCancelled else { return }
            searchResults = []
            showEmptyState = searchResults.isEmpty
            return
          } else {
            searchResults = results
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
    .navigationDestination(
      for: Entry.self,
    ) { entry in
      EntryDetail(rowId: entry.id)
    }
    .toolbar(shouldHideToolbar ? .hidden : .visible, for: .navigationBar)
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
    let searchTerm = searchText.lowercased()

    var results: [Entry] = []
    switch selectedOption {
    case .traditional:
      results = await searcher.searchTraditional(
        searchTerm: searchTerm,
      )
    case .simplified:
      results = await searcher.searchSimplified(
        searchTerm: searchTerm,
      )
    case .pinyin:
      results = await searcher.searchPinyin(
        searchTerm: searchTerm,
        useFuzzyPinyin: false
      )
    case .fuzzyPinyin:
      results = await searcher.searchPinyin(
        searchTerm: searchTerm,
        useFuzzyPinyin: true
      )
    case .jyutping:
      results = await searcher.searchJyutping(
        searchTerm: searchTerm,
        useFuzzyJyutping: false
      )
    case .fuzzyJyutping:
      results = await searcher.searchJyutping(
        searchTerm: searchTerm,
        useFuzzyJyutping: true
      )
    case .english:
      results = await searcher.searchEnglish(
        searchTerm: searchTerm
      )
    case .autoDetect:
      let detector = InputMethodDetector(searcher: searcher)
      async let isSimplified = detector.hasSimplified(text: searchTerm)
      async let isTraditional = detector.hasTraditional(text: searchTerm)
      async let isCharacters = detector.hasHanCharacters(text: searchTerm)

      if await isSimplified {
        detectedInputMethod = .simplified
        results = await searcher.searchSimplified(
          searchTerm: searchTerm
        )
        break
      } else if await isTraditional {
        detectedInputMethod = .traditional
        results = await searcher.searchTraditional(
          searchTerm: searchTerm
        )
        break
      } else if await isCharacters {
        detectedInputMethod = .traditional
        results = await searcher.searchTraditional(
          searchTerm: searchTerm
        )
        break
      }

      async let isJyutping = detector.hasJyutping(text: searchTerm)
      async let isPinyin = detector.hasPinyin(text: searchTerm)

      if let method = await isJyutping {
        detectedInputMethod = method
        results = await searcher.searchJyutping(
          searchTerm: searchTerm,
          useFuzzyJyutping: method == .fuzzyJyutping
        )
        break
      } else if let method = await isPinyin {
        detectedInputMethod = method
        results = await searcher.searchPinyin(
          searchTerm: searchTerm,
          useFuzzyPinyin: method == .fuzzyPinyin
        )
        break
      }

      detectedInputMethod = .english
      results = await searcher.searchEnglish(
        searchTerm: searchTerm
      )
    default:
      results = await searcher.searchJyutping(
        searchTerm: searchTerm,
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

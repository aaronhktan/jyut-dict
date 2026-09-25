//
//  SearchingView.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-22.
//

import GRDB
import SwiftUI

struct SearchingView: View {
  @Environment(DatabaseManager.self) private var databaseManager
  @Environment(\.dismissSearch) private var dismissSearch

  @Bindable var searchContext: SearchContext

  @Binding var isSearchActive: Bool
  var isSearchFocused: FocusState<Bool>.Binding

  @State private var selectedOptionLabel: String = "Auto-detect language"
  @State private var isPickerTextVisible: Bool = true

  static let options: [InputMethod] = [
    .autoDetect, .traditional, .simplified, .fuzzyJyutping,
    .jyutping, .fuzzyPinyin, .pinyin, .english,
  ]

  private var shouldHideToolbar: Bool {
    appIdiom != .pad && (isSearchActive || isSearchFocused.wrappedValue)
  }

  var body: some View {
    Group {
      if isSearchActive {
        searchResultView()
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
    #if os(iOS)
      .toolbar(shouldHideToolbar ? .hidden : .visible, for: .navigationBar)
    #endif
    .onChange(of: isSearchActive) {
      if !isSearchActive {
        searchContext.searchResults = []
      }
    }
  }

  private func triggerSearch() async -> [Entry] {
    guard let pool = databaseManager.dbPool else { return [] }
    let searcher = SQLSearch(pool: pool)
    let searchTerm = searchContext.searchText.lowercased()

    var results: [Entry] = []
    switch searchContext.selectedOption {
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
        searchContext.detectedInputMethod = .simplified
        results = await searcher.searchSimplified(
          searchTerm: searchTerm
        )
        break
      } else if await isTraditional {
        searchContext.detectedInputMethod = .traditional
        results = await searcher.searchTraditional(
          searchTerm: searchTerm
        )
        break
      } else if await isCharacters {
        searchContext.detectedInputMethod = .traditional
        results = await searcher.searchTraditional(
          searchTerm: searchTerm
        )
        break
      }

      async let isJyutping = detector.hasJyutping(text: searchTerm)
      async let isPinyin = detector.hasPinyin(text: searchTerm)

      if let method = await isJyutping {
        searchContext.detectedInputMethod = method
        results = await searcher.searchJyutping(
          searchTerm: searchTerm,
          useFuzzyJyutping: method == .fuzzyJyutping
        )
        break
      } else if let method = await isPinyin {
        searchContext.detectedInputMethod = method
        results = await searcher.searchPinyin(
          searchTerm: searchTerm,
          useFuzzyPinyin: method == .fuzzyPinyin
        )
        break
      }

      searchContext.detectedInputMethod = .english
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

extension SearchingView {
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
    if appIdiom == .pad || appIdiom == .mac {
      ToolbarItem(placement: topPlacement) {
        Button("Close search", systemImage: "xmark") {
          withAnimation(.snappy(duration: 0.4)) {
            isSearchFocused.wrappedValue = false
            isSearchActive = false
            dismissSearch()
          }
        }
        .labelsHidden()
        .glassEffect()
      }
    }
  }

  // MARK: - Search Result View
  private var autoDetectLabel: String {
    guard searchContext.selectedOption == .autoDetect, !searchContext.searchText.isEmpty else {
      return "Auto-detect language"
    }
    return "Detected input: \(InputMethodNames[searchContext.detectedInputMethod] ?? "")"
  }

  @ViewBuilder
  private func searchResultView() -> some View {
    ZStack(alignment: .bottom) {
      List(searchContext.searchResults, selection: $searchContext.selectedRowId) { entry in
        EntryRow(entry: entry)
      }
      .listStyle(.automatic)
      .scrollDismissesKeyboard(.immediately)
      .overlay {
        if searchContext.showEmptyState {
          ContentUnavailableView {
            Image(systemName: "flag.slash")
              .resizable()
              .scaledToFit()
              .frame(width: 50)
              .foregroundStyle(.accent)
              .opacity(0.5)
              .padding()
            Text("No search results were found.")
          }
        }
      }
      .safeAreaInset(edge: .bottom) {
        Picker("Search Options", selection: $searchContext.selectedOption) {
          ForEach(
            Self.options.sorted(by: {
              $0.rawValue < $1.rawValue
            }),
            id: \.self
          ) { option in
            Text(option == .autoDetect ? selectedOptionLabel : InputMethodNames[option] ?? "")
              .tag(option)
          }
        }
        .labelsHidden()
        .opacity(isPickerTextVisible ? 1 : 0)
        .glassEffect()
        .task(id: autoDetectLabel) {
          let target = autoDetectLabel
          guard target != selectedOptionLabel, searchContext.selectedOption == .autoDetect
          else {
            return
          }
          isPickerTextVisible = false
          withAnimation(.snappy(duration: 0.2)) {
            selectedOptionLabel = target
          } completion: {
            guard target == autoDetectLabel, searchContext.selectedOption == .autoDetect else {
              return
            }
            isPickerTextVisible = true
          }
        }
        .padding(.bottom, (appIdiom == .pad || appIdiom == .mac) ? 10 : 0)
      }
    }
    .task(id: searchContext.query) {
      searchContext.showEmptyState = false
      guard !searchContext.searchText.isEmpty else {
        searchContext.searchResults = []
        searchContext.detectedInputMethod = .autoDetect
        return
      }

      let results = await triggerSearch()
      guard !Task.isCancelled else { return }

      if results.isEmpty {
        // Delay showing empty screen to avoid flash in interface
        try? await Task.sleep(for: .milliseconds(500))
        guard !Task.isCancelled else { return }
        searchContext.searchResults = []
        searchContext.showEmptyState = true
        return
      } else {
        searchContext.searchResults = results
      }
    }
    .toolbar {
      toolbarContent
    }
  }
}

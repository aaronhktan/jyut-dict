//
//  ContentView.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-01.
//

import SwiftUI

struct ContentView: View {
  @Environment(DatabaseManager.self) private var databaseManager

  @State private var presentedSheet: Sheet?
  @State private var isSearchActive: Bool = false
  @State private var searchContext: SearchContext = SearchContext()

  // This is used to make sure search focus isn't lost on iPad when
  // tapping the input method picker.
  @FocusState private var isSearchFocused: Bool

  private enum Sheet: String, Identifiable {
    case transcription, handwriting, saved, history, settings

    var id: String { rawValue }
  }

  var body: some View {
    NavigationSplitView {
      SearchingView(
        searchContext: searchContext,
        isSearchActive: $isSearchActive,
        isSearchFocused: $isSearchFocused
      )
      .toolbar {
        toolbarContent
      }
      .toolbar(removing: .sidebarToggle)
      .sheet(item: $presentedSheet) {
        sheetView(for: $0)
      }
      .customSearchable(
        text: $searchContext.searchText,
        isPresented: $isSearchActive,
      )
      .searchFocused($isSearchFocused)
      .onChange(of: isSearchFocused) { _, focused in
        if appIdiom == .pad && focused {
          isSearchActive = true
        }
      }
      .searchPresentationToolbarBehavior(.avoidHidingContent)
      .navigationSplitViewColumnWidth(
        min: 300, ideal: 350, max: 600
      )
    } detail: {
      NavigationStack {
        if let rowId = searchContext.selectedRowId {
          EntryDetail(rowId: rowId)
        }
      }
    }
  }
}

extension ContentView {
  // MARK: - Toolbar
  private var topPlacement: ToolbarItemPlacement {
    #if os(iOS)
      .topBarTrailing
    #else
      .navigation
    #endif
  }

  private var bottomPlacement: ToolbarItemPlacement {
    #if os(iOS)
      .bottomBar
    #else
      .navigation
    #endif
  }

  @ToolbarContentBuilder
  private var toolbarContent: some ToolbarContent {
    if !isSearchActive {
      ToolbarItem(placement: topPlacement) {
        ControlGroup {
          Button("History", systemImage: "clock", action: { presentedSheet = .history })
            .labelStyle(.iconOnly)
          Button("Saved", systemImage: "star", action: { presentedSheet = .saved })
            .labelStyle(.iconOnly)
          Button("Settings", systemImage: "gearshape", action: { presentedSheet = .settings })
            .labelStyle(.iconOnly)
        }
      }
    }

    DefaultToolbarItem(kind: .search, placement: bottomPlacement)
    ToolbarSpacer(.fixed, placement: bottomPlacement)
    ToolbarItem(placement: bottomPlacement) {
      ControlGroup {
        Button(
          "Search by dictating", systemImage: "microphone",
          action: {
            presentedSheet = .transcription
          }
        )
        .labelStyle(.iconOnly)
        Button(
          "Search with handwriting", systemImage: "pencil.and.scribble",
          action: {
            presentedSheet = .handwriting
          }
        )
        .labelStyle(.iconOnly)
      }
    }
  }

  // MARK: - Sheets
  @ViewBuilder
  private func sheetView(for sheet: Sheet) -> some View {
    switch sheet {
    case .transcription:
      TranscriptionView()
        .presentationDetents([.medium])
    case .handwriting:
      HandwritingView()
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
}

@Observable @MainActor
final class SearchContext {
  var searchText: String = ""
  var selectedOption: InputMethod = .autoDetect
  var searchResults: [Entry] = []
  var selectedRowId: Int?
  var detectedInputMethod: InputMethod = .none
  var showEmptyState: Bool = false

  struct Query: Equatable {
    let text: String
    let option: InputMethod
  }
  var query: Query {
    Query(text: searchText, option: selectedOption)
  }
}

#Preview {
  @Previewable @State var databaseManager = DatabaseManager()
  ContentView()
    .environment(databaseManager)
}

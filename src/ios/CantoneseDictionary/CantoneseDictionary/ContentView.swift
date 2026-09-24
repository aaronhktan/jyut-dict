//
//  ContentView.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-01.
//

import SwiftUI

// On iPhone, the isPresented binding is useful to determine whether
// the search interface is active. However, on iPad, isPresented is
// set to false as soon as the picker is tapped, which makes the
// search interface dismiss itself immediately. This is undesirable.
extension View {
  @ViewBuilder
  func searchableWithPresentedOniPhoneOnly(text: Binding<String>, isPresented: Binding<Bool>)
    -> some View
  {
    #if os(iOS)
      if UIDevice.current.userInterfaceIdiom == .phone {
        self.searchable(
          text: text,
          isPresented: isPresented,
          prompt: "Search",
        )
      } else {
        self.searchable(
          text: text,
          prompt: "Search",
        )
      }
    #else
      self.searchable(
        text: text,
        prompt: "Search",
      )
    #endif
  }
}

struct ContentView: View {
  @Environment(DatabaseManager.self) private var databaseManager

  @State private var presentedSheet: Sheet?
  @State private var searchText: String = ""
  @State private var isSearchActive = false
  @State private var searchResults: [Entry] = []
  @State private var selectedRowId: Int?

  // This is used to make sure search focus isn't lost on iPad when
  // tapping the input method picker.
  @FocusState private var isSearchFocused: Bool

  private var isIPad: Bool {
    #if os(iOS)
      UIDevice.current.userInterfaceIdiom == .pad
    #else
      false
    #endif
  }

  private enum Sheet: String, Identifiable {
    case transcription, handwriting, saved, history, settings

    var id: String { rawValue }
  }

  var body: some View {
    NavigationSplitView {
      SearchingView(
        searchText: $searchText,
        isSearchActive: $isSearchActive,
        isSearchFocused: Binding(
          get: { isSearchFocused },
          set: { isSearchFocused = $0 }
        ),
        searchResults: $searchResults,
        selectedRowId: $selectedRowId,
      )
      .toolbar {
        if !isSearchActive {
          #if os(iOS)
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
          #else
            ToolbarItem(placement: .navigation) {
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
          #endif
        }
        #if os(iOS)
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
        #else
          DefaultToolbarItem(kind: .search)
          ToolbarSpacer(.fixed)
          ToolbarItem(placement: .navigation) {
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
        #endif
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
      .searchableWithPresentedOniPhoneOnly(
        text: $searchText,
        isPresented: $isSearchActive,
      )
      .searchFocused($isSearchFocused)
      .onChange(of: isSearchFocused) { _, focused in
        if isIPad && focused {
          isSearchActive = true
        }
      }
      .searchPresentationToolbarBehavior(.avoidHidingContent)
    } detail: {
      NavigationStack {
        if let selectedRowId {
          EntryDetail(rowId: selectedRowId)
        }
      }
    }
  }
}

#Preview {
  @Previewable @State var databaseManager = DatabaseManager()
  ContentView()
    .environment(databaseManager)
}

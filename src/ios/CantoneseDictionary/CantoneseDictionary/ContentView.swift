//
//  ContentView.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-01.
//

import SwiftUI

struct ContentView: View {
    enum Sheet: String, Identifiable {
        case transcription, handwriting, saved, history, settings

        var id: String { rawValue }
    }

    @Environment(DatabaseManager.self) private var databaseManager
    @State private var presentedSheet: Sheet?
    @State private var searchText: String = ""
    @State private var searchIsActive = false
    @State private var processedSearchText: [String] = []

    var body: some View {
        NavigationSplitView {
            SearchingView(
                searchText: $searchText,
                isSearchActive: $searchIsActive,
                processedSearchText: $processedSearchText,
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
        } detail: {
            Text("Hi")
        }
    }
}

#Preview {
    @Previewable @State var databaseManager = DatabaseManager()
    ContentView()
        .environment(databaseManager)
}

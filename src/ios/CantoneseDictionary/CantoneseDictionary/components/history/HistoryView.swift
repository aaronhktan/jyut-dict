//
//  HistoryView.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-08.
//

import SwiftUI

struct HistoryView: View {
  @Environment(\.dismiss) var dismiss
  @Environment(DatabaseManager.self) private var databaseManager

  @Bindable var searchContext: SearchContext

  @Binding var isSearchActive: Bool

  @State private var selectedHistoryType: HistoryType = .searchHistory
  @State private var historyContext = HistoryContext()

  private enum HistoryType: CaseIterable, Identifiable {
    case searchHistory, viewHistory

    var descriptionString: String {
      switch self {
      case .searchHistory:
        return "Search History"
      case .viewHistory:
        return "Viewed Entries"
      }
    }

    var id: Self { self }
  }

  var body: some View {
    NavigationStack {
      VStack {
        Picker("Select history to view", selection: $selectedHistoryType) {
          ForEach(HistoryType.allCases) { type in
            Text(type.descriptionString)
          }
        }
        .pickerStyle(.segmented)
        .padding(.horizontal)

        switch selectedHistoryType {
        case .searchHistory:
          if let searchedTerms = historyContext.searchedTerms {
            if searchedTerms.isEmpty {
              ContentUnavailableView {
                Image(systemName: "exclamationmark.magnifyingglass")
                  .resizable()
                  .scaledToFit()
                  .frame(width: 50)
                  .foregroundStyle(.accent)
                  .opacity(0.5)
                  .padding()
                Text("No searches have been made yet!")
              }
            } else {
              List(searchedTerms) { item in
                Button {
                  searchContext.searchText = item.searchTerm
                  searchContext.selectedOption = item.inputMethod
                  isSearchActive = true

                  dismiss()
                } label: {
                  SearchHistoryRow(historyItem: item)
                    .frame(maxWidth: .infinity, alignment: .leading)
                    .contentShape(Rectangle())
                }
                .buttonStyle(.plain)
                .swipeActions(edge: .trailing) {
                  Button(role: .destructive) {
                    guard let pool = databaseManager.dbPool else { return }
                    let userHistory = SQLUserHistory(pool: pool)
                    Task {
                      await userHistory.removeSearchFromHistory(search: item)
                    }
                  } label: {
                    Label("Remove from history", systemImage: "trash")
                  }
                }
              }
              .contentMargins(.top, 5)
            }
          } else {
            if historyContext.showLoading {
              ContentUnavailableView {
                VStack {
                  Text("Loading history...")
                  ProgressView()
                    .progressViewStyle(CircularProgressViewStyle(tint: .accent))
                    .controlSize(.extraLarge)
                    .transition(.scale(scale: 0.8).combined(with: .opacity))
                }
              }
            } else {
              Spacer()
            }
          }
        case .viewHistory:
          if let viewedEntries = historyContext.viewedEntries {
            if viewedEntries.isEmpty {
              ContentUnavailableView {
                Image(systemName: "eye.slash")
                  .resizable()
                  .scaledToFit()
                  .frame(width: 50)
                  .foregroundStyle(.accent)
                  .opacity(0.5)
                  .padding()
                Text("No entries have been viewed yet!")
              }
            } else {
              List(viewedEntries, selection: $historyContext.selectedRowId) { entry in
                NavigationLink {
                  EntryDetail(rowId: entry.id)
                } label: {
                  EntryRow(entry: entry)
                }
                .swipeActions(edge: .trailing) {
                  Button(role: .destructive) {
                    guard let pool = databaseManager.dbPool else { return }
                    let userHistory = SQLUserHistory(pool: pool)
                    Task {
                      await userHistory.removeViewFromHistory(entry: entry)
                    }
                  } label: {
                    Label("Remove from history", systemImage: "trash")
                  }
                }
              }
              .contentMargins(.top, 5)
            }
          } else {
            if historyContext.showLoading {
              ContentUnavailableView {
                VStack {
                  Text("Loading history...")
                  ProgressView()
                    .progressViewStyle(CircularProgressViewStyle(tint: .accent))
                    .controlSize(.extraLarge)
                    .transition(.scale(scale: 0.8).combined(with: .opacity))
                }
              }
            } else {
              Spacer()
            }
          }
        }
      }
      .navigationTitle("History")
      .navigationBarTitleDisplayMode(.inline)
      .toolbar {
        #if os(iOS)
          ToolbarItem(placement: .topBarLeading) {
            Button("Close history", systemImage: "xmark") {
              dismiss()
            }
            .labelsHidden()
          }
        #else
          ToolbarItem(placement: .navigation) {
            Button("Close history", systemImage: "xmark") {
              dismiss()
            }
            .labelsHidden()
          }
        #endif
      }
    }
    .task {
      guard let pool = databaseManager.dbPool else { return }
      let userHistory = SQLUserHistory(pool: pool)
      async let searchHistory = userHistory.fetchSearchHistory()
      async let viewedEntries = userHistory.fetchViewHistory()
      (historyContext.searchedTerms, historyContext.viewedEntries) = await (
        searchHistory, viewedEntries
      )
    }
    .onAppear {
      Task {
        try! await Task.sleep(for: .seconds(1))
        if historyContext.searchedTerms != nil {
          return
        }
        historyContext.showLoading = true
      }
    }

  }
}

@Observable @MainActor
final class HistoryContext {
  var showLoading = false
  var searchedTerms: [SearchHistoryItem]? = nil
  var viewedEntries: [Entry]? = nil
  var selectedRowId: Int?
}

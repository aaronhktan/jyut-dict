//
//  SavedView.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-08.
//

import SwiftUI

struct SavedView: View {
  @Environment(\.dismiss) var dismiss
  @Environment(DatabaseManager.self) private var databaseManager

  @State private var savedContext = SavedContext()

  var body: some View {
    NavigationStack {
      VStack {
        if let savedEntries = savedContext.savedEntries {
          if savedEntries.isEmpty {
            ContentUnavailableView {
              Image(systemName: "star.slash")
                .resizable()
                .scaledToFit()
                .frame(width: 50)
                .foregroundStyle(.accent)
                .opacity(0.5)
                .padding()
              Text("No entries have been saved yet!")
            }
          } else {
            List(savedEntries, selection: $savedContext.selectedRowId) { entry in
              NavigationLink {
                EntryDetail(rowId: entry.id)
              } label: {
                EntryRow(entry: entry, isDeletable: true)
              }
            }
            .contentMargins(.top, 0)
          }
        } else {
          if savedContext.showLoading {
            ContentUnavailableView {
              VStack {
                Text("Loading saved entries...")
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
      .navigationTitle("Saved Entries")
      .navigationBarTitleDisplayMode(.inline)
      .toolbar {
        #if os(iOS)
          ToolbarItem(placement: .topBarLeading) {
            Button("Close saved words", systemImage: "xmark") {
              dismiss()
            }
            .labelsHidden()
          }
        #else
          ToolbarItem(placement: .navigation) {
            Button("Close saved words", systemImage: "xmark") {
              dismiss()
            }
            .labelsHidden()
          }
        #endif
      }
    }
    .task {
      guard let pool = databaseManager.dbPool else { return }
      let userData = SQLUserData(pool: pool)
      savedContext.savedEntries = await userData.fetchSaved()
    }
    .onAppear {
      Task {
        try! await Task.sleep(for: .seconds(1))
        if savedContext.savedEntries != nil {
          return
        }
        savedContext.showLoading = true
      }
    }

  }
}

@Observable @MainActor
final class SavedContext {
  var savedEntries: [Entry]? = nil
  var selectedRowId: Int?
  var showLoading = false
}

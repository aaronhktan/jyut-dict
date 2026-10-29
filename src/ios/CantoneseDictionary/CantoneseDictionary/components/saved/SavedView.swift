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
        if savedContext.savedEntries.isEmpty {
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
          List(savedContext.savedEntries, selection: $savedContext.selectedRowId) { entry in
            NavigationLink {
              EntryDetail(rowId: entry.id)
            } label: {
              EntryRow(entry: entry)
            }
          }
          .contentMargins(.top, 0)
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

  }
}

@Observable @MainActor
final class SavedContext {
  var savedEntries: [Entry] = []
  var selectedRowId: Int?
}

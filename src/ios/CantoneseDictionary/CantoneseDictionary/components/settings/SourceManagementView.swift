//
//  SourceManagementView.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-10-24.
//

import SwiftUI
import os

struct SourceManagementView: View {
  @Environment(DatabaseManager.self) private var databaseManager
  @Environment(\.openURL) var openURL

  @State private var sourceModel = SourceModel()
  @State private var selectedSource: Int?

  var body: some View {
    NavigationStack {
      List(sourceModel.sources, selection: $selectedSource) { source in
        NavigationLink {
          VStack {
            List {
              VStack(alignment: .leading, spacing: 2) {
                Text("About")
                  .fontWeight(.bold)
                  .foregroundStyle(.placeholder)
                Text(source.description)
                  .multilineTextAlignment(.leading)
              }

              VStack(alignment: .leading, spacing: 2) {
                Text("Licence")
                  .fontWeight(.bold)
                  .foregroundStyle(.placeholder)
                Text(source.legal)
                  .multilineTextAlignment(.leading)
              }

              VStack(alignment: .leading, spacing: 2) {
                Text("Version")
                  .fontWeight(.bold)
                  .foregroundStyle(.placeholder)
                Text(source.version)
                  .multilineTextAlignment(.leading)
              }
            }

            Button {
              openURL(URL(string: source.link)!)
            } label: {
              Label("Visit website", systemImage: "globe")
                .frame(maxWidth: .infinity)
            }
            .buttonStyle(.glass)
            .tint(.accent)
            .buttonBorderShape(.capsule)
            .controlSize(.extraLarge)
            .padding(.horizontal)
          }
          .navigationTitle(source.name)
        } label: {
          Text(source.name)
        }
      }
      .listStyle(.automatic)
      .contentMargins(.top, 0)
    }
    .task {
      await sourceModel.loadSources(databaseManager)
    }
    .navigationTitle("Sources")
  }
}

@Observable
class SourceModel {
  var sources: [SourceMetadata] = []

  func loadSources(_ databaseManager: DatabaseManager) async {
    guard let pool = databaseManager.dbPool else { return }
    let searcher = SQLSourceMetadata(pool: pool)
    sources = await searcher.getSources()
  }
}

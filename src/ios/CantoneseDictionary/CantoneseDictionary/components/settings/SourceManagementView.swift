//
//  SourceManagementView.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-10-24.
//

import SwiftUI
import UniformTypeIdentifiers
import os

struct SourceManagementView: View {
  @Environment(DatabaseManager.self) private var databaseManager

  @State private var sourceModel = SourceModel()
  @State private var utils = SQLDatabaseUtils()
  @State private var isFileImportPresented = false
  @State private var isSheetPresented = false
  @State private var selectedSource: Int?

  var body: some View {
    NavigationStack {
      List(sourceModel.sources, selection: $selectedSource) { source in
        NavigationLink {
          SourceDetailView(source: source)
        } label: {
          Text(source.name)
        }
      }
      .listStyle(.automatic)
      .contentMargins(.top, 0)
      .toolbar {
        ToolbarItem(placement: .topBarTrailing) {
          Button("Add source", systemImage: "plus") {
            isFileImportPresented = true
          }
          .labelsHidden()
        }
      }
      .fileImporter(
        isPresented: $isFileImportPresented, allowedContentTypes: [.data]
      ) { result in
        switch result {
        case .success(let url):
          Task {
            guard url.startAccessingSecurityScopedResource() else {
              return
            }
            defer { url.stopAccessingSecurityScopedResource() }
            await databaseManager.checkpointDatabase()
            await databaseManager.backupDatabase()
            if let pool = databaseManager.dbPool {
              utils.setPool(pool: pool)
              isSheetPresented = true
              let succeeded = await utils.addSource(from: url, overwriteConflictingSource: false)
              if !succeeded, utils.state == .addSourceFailed, !utils.conflictingSources.isEmpty {
                // This extends the lifetime of the URL, so the user can
                // choose to overwrite the conflicting source
                guard url.startAccessingSecurityScopedResource() else {
                  return
                }
              }
              await sourceModel.loadSources(databaseManager)
            }
          }
        case .failure(let error):
          logger.error("User database file import failed: \(error)")
        }
      }
      .sheet(isPresented: $isSheetPresented) {
        sheetView()
          .presentationDetents([.large])
          .interactiveDismissDisabled()
      }
    }
    .task {
      await sourceModel.loadSources(databaseManager)
    }
    .navigationTitle("Sources")
  }
}

extension SourceManagementView {
  @ViewBuilder
  private func sheetView() -> some View {
    VStack {
      VStack(spacing: 30) {
        Spacer()

        Image("AppIconImage")
          .resizable()
          .scaledToFit()
          .frame(width: 200)
        Text(utils.state.descriptionString)
          .font(.title3.weight(.semibold))
          .multilineTextAlignment(.center)
          .padding(.horizontal)

        if utils.state != .idle, utils.state != .addSourceFailed,
          utils.state != .addSourceSucceeded
        {
          ProgressView()
            .progressViewStyle(CircularProgressViewStyle(tint: .accent))
            .controlSize(.extraLarge)
            .transition(.scale(scale: 0.8).combined(with: .opacity))
        }

        if utils.state == .addSourceFailed && !utils.conflictingSources.isEmpty {
          Text(
            "At least once source you tried to add (\(utils.conflictingSources.map({ $0.name}).joined(separator: ","))) conflicts with existing sources already in your database."
          )
          .foregroundStyle(.placeholder)
          .multilineTextAlignment(.center)
          .padding()
        } else if utils.state == .addSourceFailed, let reason = utils.reason {
          Text("\(reason.errorDescription ?? "")")
            .foregroundStyle(.placeholder)
            .multilineTextAlignment(.center)
            .padding(.horizontal)
        }

        if utils.state == .addSourceSucceeded {
          Image(systemName: "checkmark.seal")
            .font(.system(size: 48))
            .transition(.symbolEffect(.drawOn.byLayer, options: .speed(0.1)))
        }

        Spacer()
      }

      if utils.state == .addSourceFailed && !utils.conflictingSources.isEmpty {
        Button {
          Task {
            if let url = utils.url {
              defer { url.stopAccessingSecurityScopedResource() }
              _ = await utils.addSource(from: url, overwriteConflictingSource: true)
            }
          }
        } label: {
          Text("Overwrite existing sources")
            .frame(maxWidth: .infinity)
        }
        .buttonStyle(.glass)
        .tint(.accent)
        .buttonBorderShape(.capsule)
        .controlSize(.extraLarge)
        .padding(.horizontal)
      }

      if utils.state == .addSourceFailed || utils.state == .addSourceSucceeded {
        Button {
          isSheetPresented = false
          if let url = utils.url {
            url.stopAccessingSecurityScopedResource()
          }
          utils.state = .idle
        } label: {
          Text("OK")
            .frame(maxWidth: .infinity)
        }
        .buttonStyle(.glassProminent)
        .tint(.accent)
        .buttonBorderShape(.capsule)
        .controlSize(.extraLarge)
        .padding(.horizontal)
      }
    }
    .animation(.snappy, value: utils.state)
  }
}

struct SourceDetailView: View {
  @Environment(DatabaseManager.self) private var databaseManager
  @Environment(\.dismiss) var dismiss
  @Environment(\.openURL) var openURL

  @State private var utils = SQLDatabaseUtils()
  @State private var isSheetPresented = false
  @State private var isBookDrawn = false
  @State private var isConfirmationShown = false

  let source: SourceMetadata

  var body: some View {
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
      .listStyle(.automatic)
      .contentMargins(.top, 0)

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
    .toolbar {
      ToolbarItem(placement: .topBarTrailing) {
        Button {
          isConfirmationShown = true
        } label: {
          Label("Delete source", systemImage: "trash")
            .frame(maxWidth: .infinity)
        }
        .labelsHidden()
        .confirmationDialog(
          "Are you sure you want to remove this source?", isPresented: $isConfirmationShown,
          titleVisibility: .visible
        ) {
          Button("Yes", role: .destructive) {
            Task {
              await databaseManager.checkpointDatabase()
              await databaseManager.backupDatabase()
              if let pool = databaseManager.dbPool {
                utils.setPool(pool: pool)
                isSheetPresented = true
                Task {
                  await utils.removeSources(sources: [source.name], skipCleanup: false)
                }
              }
            }
          }
        }
      }
    }
    .sheet(isPresented: $isSheetPresented) {
      sheetView()
        .presentationDetents([.large])
        .interactiveDismissDisabled()
    }
  }
}

extension SourceDetailView {
  @ViewBuilder
  private func sheetView() -> some View {
    VStack {
      VStack(spacing: 30) {
        Spacer()

        Image("AppIconImage")
          .resizable()
          .scaledToFit()
          .frame(width: 200)
        Text(utils.state.descriptionString)
          .font(.title3.weight(.semibold))
          .multilineTextAlignment(.center)

        if utils.state != .idle, utils.state != .removeSourceFailed,
          utils.state != .removeSourceSucceeded
        {
          ProgressView()
            .progressViewStyle(CircularProgressViewStyle(tint: .accent))
            .controlSize(.extraLarge)
            .transition(.scale(scale: 0.8).combined(with: .opacity))
        }

        if utils.state == .removeSourceSucceeded {
          Image(systemName: "book.pages")
            .font(.system(size: 48))
            .symbolEffect(.drawOff, options: .speed(0.1), isActive: isBookDrawn)
            .task {
              try? await Task.sleep(for: .seconds(0.45))
              isBookDrawn = true
            }
        }

        Spacer()
      }

      if utils.state == .removeSourceFailed || utils.state == .removeSourceSucceeded {
        Button {
          isSheetPresented = false
          if utils.state == .removeSourceSucceeded {
            dismiss()
          }
          utils.state = .idle
        } label: {
          Text("OK")
            .frame(maxWidth: .infinity)
        }
        .buttonStyle(.glassProminent)
        .tint(.accent)
        .buttonBorderShape(.capsule)
        .controlSize(.extraLarge)
        .padding(.horizontal)
      }
    }
    .animation(.snappy, value: utils.state)
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

//
//  AdvancedView.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-10-29.
//

import SwiftUI
import UniformTypeIdentifiers
import os

struct AdvancedView: View {
  @Environment(DatabaseManager.self) private var databaseManager

  @State private var showDatabaseExport = false
  @State private var showDatabaseImport = false
  @State private var showUserDatabaseExport = false
  @State private var showUserDatabaseImport = false
  @State private var isGatheringLogs = false
  @State private var showExportLogs = false
  @State private var isFailureShown = false
  @State private var logs = LogStore()

  var body: some View {
    List {
      Button("Export dictionary database") {
        Task {
          await databaseManager.checkpointDatabase()
          showDatabaseExport = true
        }
      }
      .tint(.primary)
      .fileExporter(
        isPresented: $showDatabaseExport,
        document: DatabaseDocument(url: databaseManager.dbURL),
        defaultFilename: "dict.db"
      ) { result in
        switch result {
        case .success(let url):
          logger.info("Saved database to \(url)")
        case .failure(let error):
          logger.warning("Export failed: \(error)")
        }
      }
      Button("Import dictionary database") {
        showDatabaseImport = true
      }
      .tint(.primary)
      .fileImporter(
        isPresented: $showDatabaseImport, allowedContentTypes: [.data]
      ) { result in
        switch result {
        case .success(let url):
          Task {
            guard url.startAccessingSecurityScopedResource() else {
              return
            }
            defer { url.stopAccessingSecurityScopedResource() }
            await databaseManager.deleteDatabase()
            let success = await databaseManager.importDatabase(from: url)
            if success {
              databaseManager.setupDatabase()
            } else {
              isFailureShown = true
            }
          }
        case .failure(let error):
          logger.error("User database file import failed: \(error)")
        }
      }
      Button("Restore from most recent dictionary database backup") {
        Task {
          let success = await databaseManager.restoreDatabase()
          if success {
            databaseManager.setupDatabase()
          } else {
            isFailureShown = true
          }
        }
      }
      .tint(.primary)
      Button("Erase all dictionary database backups") {
        Task {
          let success = await databaseManager.deleteBackups()
          if success {
            databaseManager.setupDatabase()
          } else {
            isFailureShown = true
          }
        }
      }
      .tint(.primary)
      Button("Restore dictionary database to default") {
        Task {
          await databaseManager.deleteDatabase()
          await databaseManager.expandDatabaseFromBundle()
          databaseManager.setupDatabase()
        }
      }
      .tint(.primary)
      Button("Export history and favourites database") {
        Task {
          await databaseManager.checkpointUserDatabase()
          showUserDatabaseExport = true
        }
      }
      .tint(.primary)
      .fileExporter(
        isPresented: $showUserDatabaseExport,
        document: DatabaseDocument(url: databaseManager.userURL),
        defaultFilename: "user.db"
      ) { result in
        switch result {
        case .success(let url):
          logger.info("Saved user database to \(url)")
        case .failure(let error):
          logger.warning("Export failed: \(error)")
        }
      }
      Button("Import history and favourites database") {
        showUserDatabaseImport = true
      }
      .tint(.primary)
      .fileImporter(
        isPresented: $showUserDatabaseImport, allowedContentTypes: [.data]
      ) { result in
        switch result {
        case .success(let url):
          Task {
            guard url.startAccessingSecurityScopedResource() else {
              return
            }
            defer { url.stopAccessingSecurityScopedResource() }
            await databaseManager.deleteUserDatabase()
            await databaseManager.importUserDatabase(from: url)
            databaseManager.setupDatabase()
          }
        case .failure(let error):
          logger.error("User database file import failed: \(error)")
        }
      }
      Button("Erase all history and favourites") {
        Task {
          await databaseManager.deleteUserDatabase()
          await databaseManager.copyUserDatabase()
          databaseManager.setupDatabase()
        }
      }
      .tint(.primary)
      Button {
        isGatheringLogs = true
        Task {
          await logs.export()
          isGatheringLogs = false
          showExportLogs = true
        }
      } label: {
        if isGatheringLogs {
          HStack {
            Text("Gathering logs...")
            ProgressView()
              .controlSize(.small)
          }
        } else if showExportLogs {
          Text("Logs gathered!")
        } else {
          Text("Export logs")
        }
      }
      .tint(.primary)
      if showExportLogs, let path = logs.filteredLogPath {
        ShareLink(item: path) {
          Text("Share logs")
        }
      }
    }
    .contentMargins(.top, 0)
    .alert(isPresented: $isFailureShown) {
      Alert(
        title: Text("Operation failed"),
        dismissButton: .default(Text("OK")))
    }
    .navigationTitle("Advanced")
  }
}

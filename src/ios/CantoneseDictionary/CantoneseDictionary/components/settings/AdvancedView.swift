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
  @State private var exportLogsText = "Export logs"
  @State private var showExportLogs = false
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
            await databaseManager.importDatabase(from: url)
            databaseManager.setupDatabase()
          }
        case .failure(let error):
          logger.error("User database file import failed: \(error)")
        }
      }
      Button("Restore dictionary database to default") {
        Task {
          await databaseManager.deleteDatabase()
          await databaseManager.copyDatabase()
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
      Button(exportLogsText) {
        exportLogsText = "Gathering logs..."
        Task {
          await logs.export()
          showExportLogs = true
          exportLogsText = "Logs gathered!"
        }
      }
      .tint(.primary)
      if showExportLogs {
        ShareLink(item: String(logs.entries.joined(separator: "\n"))) {
          Text("Share logs")
        }
      }
    }
    .contentMargins(.top, 0)
    .navigationTitle("Advanced")
  }
}

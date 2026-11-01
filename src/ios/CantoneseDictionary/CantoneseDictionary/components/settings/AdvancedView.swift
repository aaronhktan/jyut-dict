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
  @State private var isPreparingDatabaseForExport = false
  @State private var showDatabaseImport = false
  @State private var showRestoreBackupConfirmation = false
  @State private var eraseBackupText = "Erase all dictionary database backups"
  @State private var showEraseBackupConfirmation = false
  @State private var showRestoreDefaultConfirmation = false
  @State private var showUserDatabaseExport = false
  @State private var showUserDatabaseImport = false
  @State private var eraseUserText = "Erase all history and favourites"
  @State private var showEraseUserConfirmation = false
  @State private var isGatheringLogs = false
  @State private var showExportLogs = false
  @State private var isFailureShown = false
  @State private var logs = LogStore()

  var body: some View {
    List {
      Section(header: Text("Dictionary Database")) {
        Button {
          Task {
            isPreparingDatabaseForExport = true
            await databaseManager.checkpointDatabase()
            showDatabaseExport = true
          }
        } label: {
          if isPreparingDatabaseForExport {
            HStack {
              Text("Preparing for export...")
              ProgressView()
                .controlSize(.small)
            }
          } else {
            Text("Export dictionary database")
          }
        }
        .tint(.primary)
        .frame(maxWidth: .infinity, alignment: .leading)
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
        .onChange(of: showDatabaseExport) { _, newValue in
          if !newValue {
            isPreparingDatabaseForExport = false
          }
        }
        Button("Import dictionary database") {
          showDatabaseImport = true
        }
        .tint(.primary)
        .frame(maxWidth: .infinity, alignment: .leading)
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
          showRestoreBackupConfirmation = true
        }
        .tint(.primary)
        .frame(maxWidth: .infinity, alignment: .leading)
        .confirmationDialog("Are you sure?", isPresented: $showRestoreBackupConfirmation, titleVisibility: .visible) {
          Button("Restore", role: .destructive) {
            Task {
              let success = await databaseManager.restoreDatabase()
              if success {
                databaseManager.setupDatabase()
              } else {
                isFailureShown = true
              }
            }
          }
          Button("Cancel", role: .cancel) {
            showRestoreBackupConfirmation = false
          }
        }
        Button(eraseBackupText) {
          showEraseBackupConfirmation = true
        }
        .tint(.primary)
        .frame(maxWidth: .infinity, alignment: .leading)
        .confirmationDialog("Are you sure?", isPresented: $showEraseBackupConfirmation, titleVisibility: .visible) {
          Button("Erase", role: .destructive) {
            Task {
              let success = await databaseManager.deleteBackups()
              if success {
                eraseBackupText = "Backups erased!"
              } else {
                isFailureShown = true
              }
            }
          }
          Button("Cancel", role: .cancel) {
            showEraseBackupConfirmation = false
          }
        }
        Button("Restore dictionary database to default") {
          showRestoreDefaultConfirmation = true
        }
        .tint(.primary)
        .frame(maxWidth: .infinity, alignment: .leading)
        .confirmationDialog("Are you sure?", isPresented: $showRestoreDefaultConfirmation, titleVisibility: .visible) {
          Button("Restore", role: .destructive) {
            Task {
              await databaseManager.deleteDatabase()
              await databaseManager.expandDatabaseFromBundle()
              databaseManager.setupDatabase()
            }
          }
          Button("Cancel", role: .cancel) {
            showRestoreDefaultConfirmation = false
          }
        }
      }

      Section(header: Text("History and Favourites")) {
        Button("Export history and favourites database") {
          Task {
            await databaseManager.checkpointUserDatabase()
            showUserDatabaseExport = true
          }
        }
        .tint(.primary)
        .frame(maxWidth: .infinity, alignment: .leading)
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
        .frame(maxWidth: .infinity, alignment: .leading)
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
        Button(eraseUserText) {
          showEraseUserConfirmation = true
        }
        .tint(.primary)
        .frame(maxWidth: .infinity, alignment: .leading)
        .confirmationDialog("Are you sure?", isPresented: $showEraseUserConfirmation, titleVisibility: .visible) {
          Button("Erase", role: .destructive) {
            Task {
              await databaseManager.deleteUserDatabase()
              await databaseManager.copyUserDatabase()
              databaseManager.setupDatabase()
              eraseUserText = "History and favourites erased!"
            }
          }
          Button("Cancel", role: .cancel) {
            showEraseUserConfirmation = false
          }
        }
      }

      Section(header: Text("Logs")) {
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
        .frame(maxWidth: .infinity, alignment: .leading)
        if showExportLogs, let path = logs.filteredLogPath {
          ShareLink(item: path) {
            Text("Share logs")
          }
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

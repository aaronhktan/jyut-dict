//
//  LogStore.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-10-28.
//

import Foundation
import OSLog
import os

// Adapted from https://swiftwithmajid.com/2022/04/19/exporting-data-from-unified-logging-system-in-swift/
final class LogStore: Observable {
  var entries: [String] = []

  @concurrent func export() async {
    do {
      logger.info("Exporting logs...")
      let store = try OSLogStore(scope: .currentProcessIdentifier)
      let date = Date.now.addingTimeInterval(-24 * 3600)
      let position = store.position(date: date)

      let filtered =
        try store
        .getEntries(at: position)
        .compactMap { $0 as? OSLogEntryLog }
        .filter { $0.subsystem == Bundle.main.bundleIdentifier! }
        .map { "[\($0.date.formatted())] [\($0.category)] \($0.composedMessage)" }

      Task { @MainActor in
        self.entries = filtered
      }
    } catch {
      logger.warning("\(error.localizedDescription, privacy: .public)")
    }
  }
}

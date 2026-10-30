//
//  SQLDatabaseUtils.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-10-29.
//

import Foundation
import GRDB
import os

@Observable final class SQLDatabaseUtils {
  private var pool: DatabasePool?

  public enum State {
    case idle
    case removingSource
    case droppingIndices
    case removingDefinitions
    case removingExamples
    case rebuildingIndices
    case removeSourceFailed
    case removeSourceSucceeded

    var descriptionString: String {
      switch self {
      case .idle:
        return "Idle"
      case .removingSource:
        return "Removing source..."
      case .droppingIndices:
        return "Dropping indices..."
      case .removingDefinitions:
        return "Removing definitions..."
      case .removingExamples:
        return "Removing examples..."
      case .rebuildingIndices:
        return "Rebuilding indices..."
      case .removeSourceFailed:
        return "Failed to remove source!"
      case .removeSourceSucceeded:
        return "Source successfully removed!"
      }
    }
  }

  var state: State = .idle

  func setPool(pool: DatabasePool) {
    self.pool = pool
  }

  nonisolated private func dropIndices(_ db: Database) -> Bool {
    Task { @MainActor in
      self.state = .droppingIndices
    }
    do {
      try db.execute(literal: "DROP INDEX fk_entry_id_index")
      try db.execute(literal: "DROP INDEX entries_simplified_idx")
      try db.execute(literal: "DROP INDEX entries_jyutping_idx")
      try db.execute(literal: "DROP INDEX entries_pinyin_idx")
      try db.execute(literal: "DROP INDEX del_fk_example_idx")
      try db.execute(literal: "DROP INDEX example_links_fk_example_translation_idx")
      try db.execute(literal: "DELETE FROM definitions_fts")
      try db.execute(literal: "DELETE FROM entries_fts")
    } catch {
      logger.error("Could not drop indices, error: \(error)")
      return false
    }
    return true
  }

  nonisolated private func rebuildIndices(_ db: Database) -> Bool {
    Task { @MainActor in
      self.state = .rebuildingIndices
    }
    do {
      try db.execute(
        literal: """
          INSERT INTO entries_fts (rowid, pinyin, jyutping) 
          SELECT rowid, pinyin, jyutping FROM entries
          """)
      try db.execute(
        literal: """
          INSERT INTO definitions_fts (rowid, fk_entry_id, definition) 
          SELECT rowid, fk_entry_id, definition FROM definitions
          """)
      try db.execute(literal: "CREATE INDEX fk_entry_id_index ON definitions(fk_entry_id)")
      try db.execute(
        literal: "CREATE INDEX IF NOT EXISTS entries_simplified_idx ON entries(simplified)")
      try db.execute(
        literal: "CREATE INDEX IF NOT EXISTS entries_jyutping_idx ON entries(jyutping)")
      try db.execute(literal: "CREATE INDEX IF NOT EXISTS entries_pinyin_idx ON entries(pinyin)")
      try db.execute(
        literal:
          "CREATE INDEX IF NOT EXISTS del_fk_example_idx ON definitions_examples_links(fk_example_id)"
      )
      try db.execute(
        literal:
          "CREATE INDEX IF NOT EXISTS example_links_fk_example_translation_idx ON example_links(fk_example_translation_id)"
      )
    } catch {
      logger.error("Could not drop indices, error: \(error)")
      return false
    }
    return true
  }

  nonisolated private func removeDefinitions(from db: Database) -> Bool {
    Task { @MainActor in
      self.state = .removingDefinitions
    }
    do {
      try db.execute(
        literal: """
          DELETE FROM entries WHERE entry_id IN 
            (SELECT entries.entry_id FROM entries 
              LEFT JOIN definitions 
              ON definitions.fk_entry_id=entries.entry_id 
              WHERE definitions.fk_entry_id IS NULL)
          """)
    } catch {
      logger.error("Could not remove definitions, error: \(error)")
      return false
    }
    return true
  }

  nonisolated private func removeExamples(from db: Database) -> Bool {
    Task { @MainActor in
      self.state = .removingExamples
    }
    do {
      try db.execute(
        literal: """
          DELETE FROM examples 
          WHERE examples.example_id IN 
          (SELECT examples.example_id 
             FROM examples 
             LEFT JOIN example_links 
               ON examples.example_id = 
                 example_links.fk_example_id 
             LEFT JOIN definitions_examples_links 
               ON examples.example_id = 
                 definitions_examples_links.fk_example_id 
             WHERE example_links.fk_example_id IS NULL 
               AND 
               definitions_examples_links.fk_example_id IS 
              NULL 
              )
          """)
      try db.execute(
        literal: """
          DELETE FROM example_translations 
          WHERE example_translations.example_translation_id IN 
          (SELECT example_translations.example_translation_id 
             FROM example_translations LEFT JOIN example_links 
             ON example_translations.example_translation_id = 
               example_links.fk_example_translation_id 
             WHERE example_links.fk_example_translation_id IS NULL)
          """)
    } catch {
      logger.error("Could not remove examples, error: \(error)")
      return false
    }
    return true
  }

  nonisolated private func failCleanup(_ db: Database, error: String) {
    do {
      try db.execute(literal: "ROLLBACK")
      try db.execute(literal: "PRAGMA foreign_keys = OFF")
      logger.error("\(error)")
    } catch {
      logger.error("Failed to roll back!")
    }
    Task { @MainActor in
      self.state = .removeSourceFailed
    }
  }

  @concurrent func removeSources(sources: [String], skipCleanup: Bool) async {
    if sources.isEmpty {
      logger.warning("Attempted to remove zero sources!")
      return
    }

    await MainActor.run {
      self.state = .removingSource
    }

    logger.info("Beginning to remove sources \(sources)")

    guard let pool = await MainActor.run(body: { self.pool }) else {
      logger.error("Attempted to remove sources when pool was nil")
      await MainActor.run {
        self.state = .removeSourceFailed
      }
      return
    }

    do {
      try await pool.writeWithoutTransaction { db in
        try db.execute(literal: "PRAGMA foreign_keys = ON")
        try db.execute(literal: "SAVEPOINT source_removal")

        do {
          var types: Set<String> = []
          for source in sources {
            guard
              let row = try Row.fetchOne(
                db,
                sql: "SELECT other FROM sources WHERE sourcename = ?",
                arguments: [source]
              )
            else {
              failCleanup(db, error: "Could not get source types from database!")
              return
            }

            if let other = row["other"] as? String {
              if other.isEmpty {
                types.insert("words")
              }

              for type in other.components(separatedBy: ",") {
                types.insert(type)
              }
            } else {
              types.insert("words")
            }

            do {
              try db.execute(sql: "DELETE FROM sources WHERE sourcename = ?", arguments: [source])
            } catch {
              failCleanup(db, error: "Could not delete source \(source), error: \(error)!")
              return
            }
          }

          if !dropIndices(db) {
            failCleanup(db, error: "Could not drop indices!")
            return
          }

          if types.isEmpty || types.contains("words") {
            if !removeDefinitions(from: db) {
              failCleanup(db, error: "Could not remove definitions!")
              return
            }
          }
          if types.contains("examples") {
            if !removeExamples(from: db) {
              failCleanup(db, error: "Could not remove examples!")
              return
            }
          }

          if !skipCleanup, !rebuildIndices(db) {
            failCleanup(db, error: "Could not rebuild indices!")
            return
          }
        } catch {
          failCleanup(db, error: "Fatal failure while attempting to remove sources: \(error)")
          return
        }

        do {
          try db.execute(literal: "RELEASE source_removal")
        } catch {
          failCleanup(db, error: "Could not commit source_removal savepoint: \(error)")
          return
        }

        do {
          try db.execute(literal: "PRAGMA foreign_keys = OFF")
        } catch {
          logger.warning("Non-fatal error: could not set foreign_keys off after removing source!")
        }

        if !skipCleanup {
          do {
            try db.execute(literal: "VACUUM")
          } catch {
            logger.warning("Non-fatal error: could not vacuum after removing sources!")
          }
        }

        Task { @MainActor in
          self.state = .removeSourceSucceeded
          logger.info("Removed sources \(sources)")
        }
      }
    } catch {
      logger.error(
        "Failed to start write without transaction while removing sources, error: \(error)!")
      await MainActor.run {
        self.state = .removeSourceFailed
      }
    }
  }
}

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

  public struct ConflictingSource {
    var name: String
    var existingVersion: String
    var newVersion: String
  }

  public enum State {
    case idle
    case removingSource
    case addingSource
    case droppingIndices
    case rebuildingIndices
    case insertingEntries
    case removingDefinitions
    case insertingDefinitions
    case removingExamples
    case insertingExamples
    case removeSourceFailed
    case removeSourceSucceeded
    case addSourceFailed
    case addSourceSucceeded

    var descriptionString: String {
      switch self {
      case .idle:
        return "Idle"
      case .removingSource:
        return "Removing source..."
      case .addingSource:
        return "Adding source..."
      case .droppingIndices:
        return "Dropping search indices..."
      case .rebuildingIndices:
        return "Rebuilding search indices..."
      case .insertingEntries:
        return "Inserting entries..."
      case .removingDefinitions:
        return "Removing definitions..."
      case .insertingDefinitions:
        return "Inserting definitions..."
      case .removingExamples:
        return "Removing examples..."
      case .insertingExamples:
        return "Inserting examples..."
      case .removeSourceFailed:
        return "Failed to remove source!"
      case .removeSourceSucceeded:
        return "Source successfully removed!"
      case .addSourceFailed:
        return "Failed to add source!"
      case .addSourceSucceeded:
        return "Source successfully added!"
      }
    }
  }

  public enum SQLError: LocalizedError {
    case sourceContentsInvalid
    case sourceDeletionFailure
    case sourceInsertionFailure
    case savepointReleaseFailure
    case savepointCreateFailure
    case indexDropFailure
    case indexRebuildFailure
    case entryInsertFailure
    case definitionRemoveFailure
    case definitionInsertFailure
    case exampleRemoveFailure
    case exampleInsertFailure
    case versionReadFailure
    case versionInvalid
    case databaseDetachFailure
    case databaseAttachFailure
    case databaseSourceInvalid
    case databaseSourceConflict

    var errorDescription: String {
      switch self {
      case .sourceContentsInvalid:
        return "Could not get source types from database!"
      case .sourceDeletionFailure:
        return "Failed to remove source!"
      case .sourceInsertionFailure:
        return "Failed to insert source!"
      case .savepointReleaseFailure:
        return "Could not release savepoint!"
      case .savepointCreateFailure:
        return "Could not create savepoint!"
      case .indexDropFailure:
        return "Could not drop search indices!"
      case .indexRebuildFailure:
        return "Could not rebuild search indices!"
      case .entryInsertFailure:
        return "Could not insert entries!"
      case .definitionRemoveFailure:
        return "Could not remove definitions!"
      case .definitionInsertFailure:
        return "Could not insert definitions!"
      case .exampleRemoveFailure:
        return "Could not remove examples!"
      case .exampleInsertFailure:
        return "Could not insert examples!"
      case .versionReadFailure:
        return "Could not read version of attached database!"
      case .versionInvalid:
        return "Dictionary version is not supported!"
      case .databaseDetachFailure:
        return "Could not detach database!"
      case .databaseAttachFailure:
        return "Could not attach database from file!"
      case .databaseSourceInvalid:
        return "Attached database's sources were invalid!"
      case .databaseSourceConflict:
        return "At least one source you tried to add already exists!"
      }
    }
  }

  var state: State = .idle
  var reason: SQLError?
  var url: URL?
  var conflictingSources: [ConflictingSource] = []

  func setPool(pool: DatabasePool) {
    self.pool = pool
  }

  nonisolated private func dropIndices(_ db: Database) -> Bool {
    Task { @MainActor in
      self.state = .droppingIndices
    }
    do {
      try db.execute(literal: "DROP INDEX IF EXISTS main.fk_entry_id_index")
      try db.execute(literal: "DROP INDEX IF EXISTS main.entries_simplified_idx")
      try db.execute(literal: "DROP INDEX IF EXISTS main.entries_jyutping_idx")
      try db.execute(literal: "DROP INDEX IF EXISTS main.entries_pinyin_idx")
      try db.execute(literal: "DROP INDEX IF EXISTS main.del_fk_example_idx")
      try db.execute(literal: "DROP INDEX IF EXISTS main.example_links_fk_example_translation_idx")
      try db.execute(literal: "DELETE FROM main.definitions_fts")
      try db.execute(literal: "DELETE FROM main.entries_fts")
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

  nonisolated private func removeFailCleanup(_ db: Database, error: String) {
    logger.error("\(error)")
    do {
      try db.execute(literal: "ROLLBACK")
    } catch {
      logger.error("Failed to roll back!")
    }

    do {
      try db.execute(literal: "PRAGMA foreign_keys = OFF")
    } catch {
      logger.error("Unable to disable foreign keys!")
    }
  }

  nonisolated private func removeSources(_ db: Database, sources: [String], skipCleanup: Bool)
    throws
  {
    if sources.isEmpty {
      logger.warning("Attempted to remove zero sources!")
      return
    }

    try db.execute(literal: "PRAGMA foreign_keys = ON")
    try db.execute(literal: "SAVEPOINT source_removal")

    var types: Set<String> = []
    for source in sources {
      guard
        let row = try Row.fetchOne(
          db,
          sql: "SELECT other FROM sources WHERE sourcename = ?",
          arguments: [source]
        )
      else {
        throw SQLError.sourceContentsInvalid
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
        throw SQLError.sourceDeletionFailure
      }
    }

    do {
      try db.execute(literal: "RELEASE source_removal")
    } catch {
      throw SQLError.savepointReleaseFailure
    }

    do {
      try db.execute(literal: "PRAGMA foreign_keys = OFF")
      try db.execute(literal: "SAVEPOINT source_deletion")
    } catch {
      throw SQLError.savepointCreateFailure
    }

    if !dropIndices(db) {
      throw SQLError.indexDropFailure
    }

    if types.isEmpty || types.contains("words") {
      if !removeDefinitions(from: db) {
        throw SQLError.definitionRemoveFailure
      }
    }
    if types.contains("examples") {
      if !removeExamples(from: db) {
        throw SQLError.exampleRemoveFailure
      }
    }

    if !skipCleanup, !rebuildIndices(db) {
      throw SQLError.indexRebuildFailure
    }

    do {
      try db.execute(literal: "RELEASE source_deletion")
    } catch {
      throw SQLError.savepointReleaseFailure
    }

    if !skipCleanup {
      do {
        try db.execute(literal: "VACUUM")
      } catch {
        logger.warning("Non-fatal error: could not vacuum after removing sources!")
      }
    }
  }

  @concurrent func removeSources(sources: [String], skipCleanup: Bool) async {
    if sources.isEmpty {
      logger.warning("Attempted to remove zero sources!")
      return
    }

    await MainActor.run {
      self.state = .removingSource
      self.reason = nil
      self.url = nil
      self.conflictingSources = []
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
        do {
          try removeSources(db, sources: sources, skipCleanup: skipCleanup)
        } catch {
          removeFailCleanup(db, error: error.localizedDescription)
          throw error
        }
      }

      await MainActor.run {
        self.state = .removeSourceSucceeded
        logger.info("Removed sources \(sources)")
      }
    } catch {
      logger.error("Failed to remove sources \(sources)")
      await MainActor.run {
        self.state = .removeSourceFailed
        if let sqlError = error as? SQLError {
          self.reason = sqlError
        }
      }
    }
  }

  nonisolated private func insertSources(_ db: Database, existingSources: [String: Int64]) -> Bool
  {
    do {
      let rows = try Row.fetchAll(
        db,
        sql: """
          SELECT sourcename, sourceshortname, version, description, legal, 
            link, update_url, other 
            FROM db.sources
          """)

      for row in rows {
        let sourcename = row["sourcename"] as! String
        let sourceshortname = row["sourceshortname"] as! String
        let version = row["version"] as! String
        let description = row["description"] as! String
        let legal = row["legal"] as! String
        let link = row["link"] as! String
        let updateURL = row["update_url"] as! String
        let other = row["other"] as! String
        if let sourceid = existingSources[sourcename] {
          try db.execute(
            sql: """
              INSERT INTO sources 
                (source_id, sourcename, sourceshortname, 
                 version, description, legal, link, 
                 update_url, other) 
                 VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
              """,
            arguments: [
              sourceid, sourcename, sourceshortname, version, description, legal, link, updateURL,
              other,
            ]
          )
        } else {
          try db.execute(
            sql: """
              INSERT INTO sources 
                (sourcename, sourceshortname, 
                 version, description, legal, link, 
                 update_url, other) 
                 VALUES (?, ?, ?, ?, ?, ?, ?, ?)
              """,
            arguments: [
              sourcename, sourceshortname, version, description, legal, link, updateURL, other,
            ]
          )
        }
      }
    } catch {
      logger.error("Failed to insert sources, error: \(error)")
      return false
    }

    return true
  }

  nonisolated private func insertEntries(_ db: Database) -> Bool {
    Task { @MainActor in
      state = .insertingEntries
    }

    do {
      try db.execute(
        literal: """
          INSERT INTO entries(traditional, simplified, pinyin, jyutping, frequency)
            SELECT traditional, simplified, pinyin, jyutping, frequency 
            FROM db.entries
          """)
    } catch {
      logger.error("Failed to insert entries, error: \(error)")
      return false
    }

    return true
  }

  nonisolated private func insertDefinitions(_ db: Database) -> Bool {
    Task { @MainActor in
      state = .insertingDefinitions
    }

    do {
      try db.execute(
        literal: """
          WITH definitions_tmp AS ( 
            SELECT entries.traditional AS traditional, 
              entries.simplified AS simplified, 
              entries.pinyin AS pinyin, entries.jyutping AS jyutping, 
              sources.sourcename AS sourcename, 
              definitions.definition AS definition, 
              definitions.label AS label 
            FROM db.entries, db.definitions, db.sources 
            WHERE db.definitions.fk_entry_id = db.entries.entry_id 
            AND db.definitions.fk_source_id = db.sources.source_id 
            ) 
           
          INSERT INTO definitions(definition, label, fk_entry_id, 
          fk_source_id) 
            SELECT d.definition, d.label, e.entry_id, s.source_id 
            FROM definitions_tmp AS d, sources AS s, entries AS e 
            WHERE d.sourcename = s.sourcename 
              AND d.traditional = e.traditional 
              AND d.simplified = e.simplified 
              AND d.pinyin = e.pinyin 
              AND d.jyutping = e.jyutping
          """)
    } catch {
      logger.error("Failed to insert definitions, error: \(error)")
      return false
    }

    return true
  }

  nonisolated private func insertExamples(_ db: Database) -> Bool {
    Task { @MainActor in
      state = .removingExamples
    }

    do {
      try db.execute(
        literal: """
          INSERT INTO examples 
              (example_id, traditional, simplified, pinyin, jyutping, language) 
            SELECT example_id, traditional, simplified, pinyin, jyutping, language 
            FROM db.examples
          """)

      try db.execute(
        literal: """
          INSERT INTO example_translations(example_translation_id, translation, language) 
            SELECT example_translation_id, translation, language 
            FROM db.example_translations
          """)

      try db.execute(
        literal: """
          WITH example_links_with_source AS ( 
            SELECT example_links.fk_example_id as fk_csi, 
              example_links.fk_example_translation_id as fk_eti, 
              sources.sourcename AS sourcename, 
              example_links.direct as direct 
            FROM db.example_links, db.sources 
            WHERE db.example_links.fk_source_id = db.sources.source_id 
            ), 
           
          example_links_with_foreign_key AS ( 
            SELECT traditional, simplified, pinyin, jyutping, language, 
              fk_eti, direct, sourcename 
            FROM example_links_with_source as elws, 
              db.examples AS e 
            WHERE elws.fk_csi = e.example_id 
            ) 
           
          INSERT INTO example_links( 
            fk_example_id, fk_example_translation_id, 
            fk_source_id, direct) 
            SELECT e.example_id, elwfk.fk_eti, 
            s.source_id, elwfk.direct 
            FROM example_links_with_foreign_key AS elwfk, sources as s, 
            examples AS e 
            WHERE s.sourcename = elwfk.sourcename 
            AND e.traditional = elwfk.traditional 
            AND e.simplified = elwfk.simplified 
            AND e.pinyin = elwfk.pinyin 
            AND e.jyutping = elwfk.jyutping 
            AND e.language = elwfk.language 
          """)

      try db.execute(
        literal: """
          WITH entry_and_definitions AS ( 
            SELECT entries.traditional AS traditional, 
              entries.simplified AS simplified, 
              entries.pinyin AS pinyin, 
              entries.jyutping AS jyutping, 
              definitions.definition AS definition, 
              definitions.label AS label, 
              definitions.definition_id AS definition_id, 
              sources.sourcename AS source 
            FROM db.entries, db.definitions, db.sources 
            WHERE db.definitions.fk_entry_id = db.entries.entry_id 
              AND db.definitions.fk_source_id = db.sources.source_id 
              ), 
           
          defs_e_links_tmp AS ( 
            SELECT 
              e.traditional AS sentence_traditional, 
              e.simplified AS sentence_simplified, 
              e.pinyin AS sentence_pinyin, 
              e.jyutping AS sentence_jyutping, 
              e.language AS sentence_language, 
              ed.definition AS definition, 
              ed.label AS label, 
              ed.traditional AS traditional, 
              ed.simplified AS simplified, 
              ed.pinyin AS pinyin, 
              ed.jyutping AS jyutping, 
              ed.source AS source 
            FROM db.definitions_examples_links AS dsl, 
              db.examples AS e, 
              entry_and_definitions AS ed 
            WHERE dsl.fk_definition_id = ed.definition_id 
              AND dsl.fk_example_id = e.example_id 
              ), 
           
          new_entry_and_definitions AS ( 
            SELECT entries.traditional AS traditional, 
              entries.simplified AS simplified, 
              entries.pinyin AS pinyin, 
              entries.jyutping AS jyutping, 
              definitions.definition AS definition, 
              definitions.label AS label, 
              definitions.definition_id AS definition_id, 
              sources.sourcename AS source 
            FROM entries, definitions, sources 
            WHERE definitions.fk_entry_id = entries.entry_id 
              AND definitions.fk_source_id = sources.source_id 
              ) 
           
          INSERT INTO definitions_examples_links( 
            fk_definition_id, fk_example_id) 
            SELECT ned.definition_id, e.example_id 
          FROM defs_e_links_tmp AS dsl, 
            new_entry_and_definitions AS ned, 
            examples AS e 
            WHERE dsl.sentence_traditional = e.traditional 
            AND dsl.sentence_simplified = e.simplified 
            AND dsl.sentence_pinyin = e.pinyin 
            AND dsl.sentence_jyutping = e.jyutping 
            AND dsl.sentence_language = e.language 
            AND dsl.definition = ned.definition 
            AND dsl.label = ned.label 
            AND dsl.traditional = ned.traditional 
            AND dsl.simplified = ned.simplified 
            AND dsl.pinyin = ned.pinyin 
            AND dsl.jyutping = ned.jyutping 
            AND dsl.source = ned.source
          """)
    } catch {
      logger.error("Failed to insert examples, error: \(error)")
      return false
    }

    return true
  }

  nonisolated private func addFailCleanup(_ db: Database, error: String) {
    logger.error("Cleaning up after failing to add database, \(error)")

    do {
      try db.execute(literal: "ROLLBACK")
    } catch {
      logger.error("Failed to roll back!")
    }

    do {
      try db.execute(literal: "DETACH DATABASE db")
    } catch {
      logger.error("Failed to detach database, error: \(error)")
    }

    do {
      try db.execute(literal: "PRAGMA foreign_keys = OFF")
    } catch {
      logger.error("Unable to disable foreign keys!")
    }
  }

  nonisolated func addSource(_ db: Database, from path: URL, overwriteConflictingSource: Bool)
    throws
  {
    do {
      try db.execute(sql: "ATTACH DATABASE ? as db", arguments: [path.path])
    } catch {
      throw SQLError.databaseAttachFailure
    }

    do {
      let version = try Int.fetchOne(db, sql: "PRAGMA db.user_version")!
      if version != currentDatabaseVersion {
        throw SQLError.versionInvalid
      }
    } catch SQLError.versionInvalid {
      throw SQLError.versionInvalid
    } catch {
      throw SQLError.versionReadFailure
    }

    var existingSources: [String: Int64] = [:]
    if overwriteConflictingSource {
      var sourcesToRemove: [String] = []
      do {
        let rows = try Row.fetchAll(
          db,
          sql: """
            SELECT source_id, sourcename 
            FROM sources WHERE sourcename IN 
              (SELECT sourcename FROM db.sources)
            """
        )
        for row in rows {
          sourcesToRemove.append(row["sourcename"] as! String)
          existingSources[row["sourcename"] as! String] = (row["source_id"] as! Int64)
        }
      } catch {
        throw SQLError.databaseSourceInvalid
      }

      do {
        try removeSources(db, sources: sourcesToRemove, skipCleanup: true)
      } catch {
        throw SQLError.sourceDeletionFailure
      }
    } else {
      do {
        let rows = try Row.fetchAll(
          db,
          sql: """
            SELECT s.sourcename, s.version AS in_database_version, 
              dbs.version AS new_file_version 
              FROM sources AS s, db.sources AS dbs 
            WHERE s.sourcename IN 
              (SELECT sourcename FROM db.sources) 
              AND s.sourcename = dbs.sourcename
            """)

        for row in rows {
          let source = ConflictingSource(
            name: row["sourcename"] as! String,
            existingVersion: row["in_database_version"] as! String,
            newVersion: row["new_file_version"] as! String)
          Task { @MainActor in
            self.conflictingSources.append(source)
          }
        }

        if !rows.isEmpty {
          throw SQLError.databaseSourceConflict
        }
      }
    }

    do {
      try db.execute(literal: "SAVEPOINT source_addition")
    } catch {
      throw SQLError.savepointCreateFailure
    }

    if !dropIndices(db) {
      throw SQLError.indexDropFailure
    }

    if !insertSources(db, existingSources: existingSources) {
      throw SQLError.sourceInsertionFailure
    }

    if !insertEntries(db) {
      throw SQLError.entryInsertFailure
    }

    if !insertDefinitions(db) {
      throw SQLError.definitionInsertFailure
    }

    if !insertExamples(db) {
      throw SQLError.exampleInsertFailure
    }

    if !rebuildIndices(db) {
      throw SQLError.indexRebuildFailure
    }

    do {
      try db.execute(literal: "RELEASE source_addition")
    } catch {
      throw SQLError.savepointReleaseFailure
    }

    do {
      try db.execute(literal: "DETACH DATABASE db")
    } catch {
      logger.warning("Non-fatal error: could not detach database after adding source")
    }
  }

  @concurrent func addSource(from path: URL, overwriteConflictingSource: Bool) async -> Bool {
    if !FileManager.default.fileExists(atPath: path.path) {
      logger.warning("Attempted to add source from path (\(path.path)) that did not point to a file!")
      await MainActor.run {
        self.state = .addSourceFailed
      }
      return false
    }

    await MainActor.run {
      self.state = .addingSource
      self.reason = nil
      self.url = path
      self.conflictingSources = []
    }

    logger.info("Beginning to add sources from \(path.path)")

    guard let pool = await MainActor.run(body: { self.pool }) else {
      logger.error("Attempted to add sources when pool was nil")
      await MainActor.run {
        self.state = .addSourceFailed
      }
      return false
    }

    do {
      try await pool.writeWithoutTransaction { db in
        do {
          try addSource(db, from: path, overwriteConflictingSource: overwriteConflictingSource)
        } catch {
          addFailCleanup(db, error: error.localizedDescription)
          throw error
        }
      }

      await MainActor.run {
        self.state = .addSourceSucceeded
        logger.info("Added sources from \(path.path)")
      }
      return true
    } catch {
      await MainActor.run {
        self.state = .addSourceFailed
        if let sqlError = error as? SQLError {
          self.reason = sqlError
        }
      }
      return false
    }
  }
}

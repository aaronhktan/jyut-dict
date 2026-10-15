//
//  SQLUserHistory.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-10-14.
//

import Foundation
import GRDB
import os

nonisolated let addSearchSQL: String = """
      INSERT INTO user.search_history
        (search_text, search_options, timestamp)
      VALUES
        (?, ?, datetime('now'))
  """

nonisolated let addViewSQL: String = """
      INSERT INTO user.view_history
        (traditional, simplified, jyutping, pinyin, timestamp)
      VALUES
        (?, ?, ?, ?, datetime('now'))
  """

nonisolated let fetchSearchesSQL: String = """
      SELECT search_text as text, search_options AS options, timestamp
      FROM user.search_history
      ORDER BY timestamp DESC
      LIMIT 1000
  """

nonisolated let fetchViewsSQL: String = """
      WITH matching_entry_ids AS (
        SELECT entry_id, timestamp
        FROM user.view_history as vh
        LEFT JOIN entries as e
        ON
          vh.traditional = e.traditional
          AND vh.simplified = e.simplified
          AND vh.pinyin = e.pinyin
          AND vh.jyutping = e.jyutping
        ORDER BY timestamp DESC
        LIMIT 1000
      ),

      matching_definition_ids AS (
        SELECT definition_id, definition FROM definitions WHERE fk_entry_id
          IN (
            SELECT entry_id FROM matching_entry_ids
          )
      ),

      matching_definitions AS (
        SELECT definition_id, fk_entry_id, fk_source_id, definition,
          label
        FROM definitions
        WHERE definitions.definition_id IN (
          SELECT definition_id FROM matching_definition_ids
        )
      ),

      matching_definition_groups AS (
        SELECT fk_entry_id,
          json_object('source', sourcename,
                      'definitions',
                      json_group_array(json_object(
                        'definition', definition))) AS definitions
        FROM matching_definitions AS md
        LEFT JOIN sources ON sources.source_id = md.fk_source_id
        GROUP BY fk_entry_id, fk_source_id
      ),

      matching_entries AS (
        SELECT entries.entry_id, simplified, traditional, jyutping, pinyin,
          json_group_array(json(definitions)) AS definitions
        FROM matching_definition_groups AS mdg
        LEFT JOIN entries ON entries.entry_id = mdg.fk_entry_id
        LEFT JOIN matching_entry_ids AS mei on mei.entry_id = mdg.fk_entry_id
        GROUP BY entries.entry_id
        ORDER BY timestamp DESC
      )

      SELECT entry_id, traditional, simplified, jyutping, pinyin, definitions FROM
        matching_entries
  """

struct SearchHistoryItem: Identifiable {
  var id = UUID()
  var searchTerm: String
  var inputMethod: InputMethod
}

actor SQLUserHistory {
  private var pool: DatabasePool

  public init(pool: DatabasePool) {
    self.pool = pool
  }

  @concurrent func addSearchToHistory(searchTerm: String, option: InputMethod) async {
    do {
      try await pool.write { db in
        try db.execute(
          sql: addSearchSQL,
          arguments: [searchTerm, option.rawValue]
        )
      }
    } catch {
      // TODO: Handle errors
      logger.error("Error happened when trying to write to user db: \(error)")
    }
  }

  @concurrent func addViewToHistory(entry: Entry) async {
    do {
      try await pool.write { db in
        try db.execute(
          sql: addViewSQL,
          arguments: [entry.traditional, entry.simplified, entry.jyutping, entry.pinyin]
        )
      }
    } catch {
      // TODO: Handle errors
      logger.error("Error happened when trying to write to user db: \(error)")
    }
  }

  @concurrent func fetchSearchHistory() async -> [SearchHistoryItem] {
    var results: [SearchHistoryItem] = []
    do {
      results = try await pool.read { db in
        let rows = try Row.fetchAll(
          db,
          sql: fetchSearchesSQL
        )
        return parseReturnedSearchHistoryRecords(rows: rows)
      }
    } catch {
      // TODO: Handle errors
      logger.error("Error happened when trying to read from db: \(error)")
    }

    return results
  }

  @concurrent func fetchViewHistory() async -> [Entry] {
    var results: [Entry] = []
    do {
      results = try await pool.read { db in
        let rows = try Row.fetchAll(
          db,
          sql: fetchViewsSQL
        )
        return parseReturnedEntryRecords(rows: rows)
      }
    } catch {
      // TODO: Handle errors
      logger.error("Error happened when trying to read from db: \(error)")
    }

    return results
  }
}

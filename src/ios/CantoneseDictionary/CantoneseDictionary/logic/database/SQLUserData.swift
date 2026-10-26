//
//  SQLUserData.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-10-14.
//

import Foundation
import GRDB
import os

nonisolated let addSavedSQL: String = """
      INSERT INTO user.favourite_words
        (traditional, simplified, jyutping, pinyin, fk_list_id, timestamp)
      VALUES
        (?, ?, ?, ?, 1, datetime('now'))
  """

nonisolated let removeSavedSQL: String = """
      DELETE FROM user.favourite_words WHERE
        traditional=? AND simplified=? AND jyutping=? AND pinyin=?
  """

nonisolated let fetchSavedByUniqueSQL: String = """
      SELECT EXISTS (SELECT 1 FROM user.favourite_words WHERE 
      traditional=? AND simplified=? AND jyutping=? AND pinyin=?) 
      AS existence
  """

nonisolated let fetchSavedSQL: String = """
      WITH matching_entry_ids AS (
        SELECT entry_id, timestamp
        FROM entries as e
        INNER JOIN user.favourite_words as fw
        ON
          fw.traditional = e.traditional
          AND fw.simplified = e.simplified
          AND fw.pinyin = e.pinyin
          AND fw.jyutping = e.jyutping
        ORDER BY timestamp DESC
        LIMIT 1000
      ),

      matching_definition_ids AS (
        SELECT definition_id, definition FROM definitions WHERE fk_entry_id
          IN (
            SELECT entry_id FROM matching_entry_ids
          )
      ),

      matching_example_ids AS (
        SELECT definition_id, fk_example_id
        FROM matching_definition_ids AS mdi
        JOIN definitions_examples_links AS del ON
          mdi.definition_id = del.fk_definition_id
      ),

      matching_translations AS (
        SELECT mei.fk_example_id,
          json_group_array(DISTINCT
            json_object('translation', translation,
                        'language', language,
                        'direct', direct
          )) AS translation
        FROM matching_example_ids AS mei
        JOIN example_links AS el ON mei.fk_example_id =
          el.fk_example_id
        JOIN example_translations AS et ON et.example_translation_id =
          el.fk_example_translation_id
        GROUP BY mei.fk_example_id
      ),

      matching_examples AS (
       SELECT example_id, traditional, simplified, pinyin,
         jyutping, language
       FROM examples AS e
       WHERE example_id IN (
         SELECT fk_example_id FROM
           matching_example_ids
         )
       ),

      matching_examples_with_translations AS (
        SELECT example_id,
          json_object('traditional', traditional,
                      'simplified', simplified,
                      'pinyin', pinyin,
                      'jyutping', jyutping,
                      'language', language,
                      'translations', json(translation)) AS example 
        FROM matching_examples AS me
        LEFT JOIN matching_translations AS mt ON me.example_id =
          mt.fk_example_id
      ),

      matching_definitions AS (
        SELECT definition_id, fk_entry_id, fk_source_id, definition,
          label
        FROM definitions
        WHERE definitions.definition_id IN (
          SELECT definition_id FROM matching_definition_ids
        )
      ),

      matching_definitions_with_examples AS (
        SELECT fk_entry_id, fk_source_id,
          json_object('definition', definition,
                      'label', label, 'examples',
                      json_group_array(json(example))) AS definition
        FROM matching_definitions AS md
        LEFT JOIN matching_example_ids AS mei ON
          md.definition_id = mei.definition_id
        LEFT JOIN matching_examples_with_translations AS mewt ON
          mei.fk_example_id = mewt.example_id
        GROUP BY md.definition_id
      ),

      matching_definition_groups AS (
        SELECT fk_entry_id,
          json_object('source', sourcename,
                      'definitions',
                      json_group_array(json(definition))) AS definitions
        FROM matching_definitions_with_examples AS mdwe
        LEFT JOIN sources ON sources.source_id = mdwe.fk_source_id
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

actor SQLUserData {
  private var pool: DatabasePool

  public init(pool: DatabasePool) {
    self.pool = pool
  }

  @concurrent func addSaved(
    traditional: String, simplified: String, jyutping: String, pinyin: String
  ) async {
    do {
      try await pool.write { db in
        try db.execute(
          sql: addSavedSQL,
          arguments: [traditional, simplified, jyutping, pinyin]
        )
      }
    } catch {
      // TODO: Handle errors
      logger.error("Error happened when trying to write to user db: \(error)")
    }
  }

  @concurrent func removeSaved(
    traditional: String, simplified: String, jyutping: String, pinyin: String
  ) async {
    do {
      try await pool.write { db in
        try db.execute(
          sql: removeSavedSQL,
          arguments: [traditional, simplified, jyutping, pinyin]
        )
      }
    } catch {
      // TODO: Handle errors
      logger.error("Error happened when trying to write to user db: \(error)")
    }
  }

  @concurrent func fetchSavedByUnique(traditional: String, simplified: String, jyutping: String, pinyin: String) async -> Bool {
    var result: Bool = false
    do {
      result = try await pool.read { db in
        let rows = try Row.fetchAll(
          db,
          sql: fetchSavedByUniqueSQL,
          arguments: [traditional, simplified, jyutping, pinyin]
        )
        return parseExistenceRecords(rows: rows)
      }
    } catch {
      // TODO: Handle errors
      logger.error("Error happened when trying to read from db: \(error)")
    }

    return result
  }

  @concurrent func fetchSaved() async -> [Entry] {
    var results: [Entry] = []
    do {
      results = try await pool.read { db in
        let rows = try Row.fetchAll(
          db,
          sql: fetchSavedSQL
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

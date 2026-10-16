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

      matching_chinese_sentence_ids AS (
        SELECT definition_id, fk_chinese_sentence_id
        FROM matching_definition_ids AS mdi
        JOIN definitions_chinese_sentences_links AS dcsl ON
          mdi.definition_id = dcsl.fk_definition_id
      ),

      matching_translations AS (
        SELECT mcsi.fk_chinese_sentence_id,
          json_group_array(DISTINCT
            json_object('sentence', sentence,
                        'language', language,
                        'direct', direct
          )) AS translation
        FROM matching_chinese_sentence_ids AS mcsi
        JOIN sentence_links AS sl ON mcsi.fk_chinese_sentence_id =
          sl.fk_chinese_sentence_id
        JOIN nonchinese_sentences AS ncs ON ncs.non_chinese_sentence_id =
          sl.fk_non_chinese_sentence_id
        GROUP BY mcsi.fk_chinese_sentence_id
      ),

      matching_sentences AS (
       SELECT chinese_sentence_id, traditional, simplified, pinyin,
         jyutping, language
       FROM chinese_sentences AS cs
       WHERE chinese_sentence_id IN (
         SELECT fk_chinese_sentence_id FROM
           matching_chinese_sentence_ids
         )
       ),

      matching_sentences_with_translations AS (
        SELECT chinese_sentence_id,
          json_object('traditional', traditional,
                      'simplified', simplified,
                      'pinyin', pinyin,
                      'jyutping', jyutping,
                      'language', language,
                      'translations', json(translation)) AS sentence
        FROM matching_sentences AS ms
        LEFT JOIN matching_translations AS mt ON ms.chinese_sentence_id =
          mt.fk_chinese_sentence_id
      ),

      matching_definitions AS (
        SELECT definition_id, fk_entry_id, fk_source_id, definition,
          label
        FROM definitions
        WHERE definitions.definition_id IN (
          SELECT definition_id FROM matching_definition_ids
        )
      ),

      matching_definitions_with_sentences AS (
        SELECT fk_entry_id, fk_source_id,
          json_object('definition', definition,
                      'label', label, 'sentences',
                      json_group_array(json(sentence))) AS definition
        FROM matching_definitions AS md
        LEFT JOIN matching_chinese_sentence_ids AS mcsi ON
          md.definition_id = mcsi.definition_id
        LEFT JOIN matching_sentences_with_translations AS mswt ON
          mcsi.fk_chinese_sentence_id = mswt.chinese_sentence_id
        GROUP BY md.definition_id
      ),

      matching_definition_groups AS (
        SELECT fk_entry_id,
          json_object('source', sourcename,
                      'definitions',
                      json_group_array(json(definition))) AS definitions
        FROM matching_definitions_with_sentences AS mdws
        LEFT JOIN sources ON sources.source_id = mdws.fk_source_id
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

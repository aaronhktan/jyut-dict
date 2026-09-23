//
//  Queries.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-21.
//

nonisolated let GLOB_OPERATOR = "GLOB"
nonisolated let REGEXP_OPERATOR = "REGEXP"

nonisolated let searchTraditionalQuery: String = """
    WITH matching_entry_ids AS (
      SELECT rowid FROM entries WHERE traditional GLOB ?
    ),

    matching_definition_ids AS (
      SELECT definition_id, definition FROM definitions WHERE fk_entry_id
        IN matching_entry_ids
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
      SELECT entry_id, simplified, traditional, jyutping, pinyin,
        json_group_array(json(definitions)) AS definitions
      FROM matching_definition_groups AS mdg
      LEFT JOIN entries ON entries.entry_id = mdg.fk_entry_id
      GROUP BY entry_id
      ORDER BY frequency DESC
    )

    SELECT entry_id, traditional, simplified, jyutping, pinyin, definitions FROM
      matching_entries
"""

nonisolated let searchSimplifiedQuery: String = """
    WITH matching_entry_ids AS (
      SELECT rowid FROM entries WHERE simplified GLOB ?
    ),

    matching_definition_ids AS (
      SELECT definition_id, definition FROM definitions WHERE fk_entry_id
        IN matching_entry_ids
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
      SELECT entry_id, simplified, traditional, jyutping, pinyin,
        json_group_array(json(definitions)) AS definitions
      FROM matching_definition_groups AS mdg
      LEFT JOIN entries ON entries.entry_id = mdg.fk_entry_id
      GROUP BY entry_id
      ORDER BY frequency DESC
    )

    SELECT entry_id, traditional, simplified, jyutping, pinyin, definitions FROM
      matching_entries
"""

nonisolated let searchJyutpingQuery: String = """
    WITH matching_entry_ids AS (
      SELECT rowid FROM entries WHERE jyutping %@ ?
    ),

    matching_definition_ids AS (
      SELECT definition_id, definition FROM definitions WHERE fk_entry_id
        IN matching_entry_ids
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
      SELECT entry_id, simplified, traditional, jyutping, pinyin,
        json_group_array(json(definitions)) AS definitions
      FROM matching_definition_groups AS mdg
      LEFT JOIN entries ON entries.entry_id = mdg.fk_entry_id
      GROUP BY entry_id
      ORDER BY frequency DESC
    )

    SELECT entry_id, traditional, simplified, jyutping, pinyin, definitions FROM
      matching_entries
    """

nonisolated let searchPinyinQuery: String = """
    WITH matching_entry_ids AS (
      SELECT rowid FROM entries WHERE pinyin %@ ?
    ),

    matching_definition_ids AS (
      SELECT definition_id, definition FROM definitions WHERE fk_entry_id
        IN matching_entry_ids
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
      SELECT entry_id, simplified, traditional, jyutping, pinyin,
        json_group_array(json(definitions)) AS definitions
      FROM matching_definition_groups AS mdg
      LEFT JOIN entries ON entries.entry_id = mdg.fk_entry_id
      GROUP BY entry_id
      ORDER BY frequency DESC
    )

    SELECT entry_id, traditional, simplified, jyutping, pinyin, definitions FROM
      matching_entries
    """

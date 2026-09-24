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

nonisolated let jyutpingExistenceQuery: String = """
      SELECT EXISTS (
        SELECT
          rowid
        FROM entries
        WHERE jyutping %@ ?
      ) AS existence
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

nonisolated let pinyinExistenceQuery: String = """
      SELECT EXISTS (
        SELECT
          rowid
        FROM entries
        WHERE pinyin %@ ?
      ) AS existence
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

nonisolated let searchEnglishQuery: String = """
  WITH matching_entry_ids AS MATERIALIZED (
    SELECT
      fk_entry_id,
      rowid AS definition_id,
      bm25(definitions_fts, 0, 1) AS RANK
    FROM definitions_fts
    WHERE definitions_fts MATCH ? AND definition LIKE ?
  ),

  matching_definition_ids AS (
    SELECT
      definition_id,
      definition
    FROM definitions
    WHERE fk_entry_id IN (
      SELECT fk_entry_id
      FROM matching_entry_ids
    )
  ),

  definitions_and_ranks AS (
    SELECT
      mdi.definition_id AS definition_id,
      mdi.definition AS definition,
      mei.rank AS RANK
    FROM matching_definition_ids AS mdi
      LEFT JOIN matching_entry_ids AS mei
        ON mdi.definition_id = mei.definition_id
  ),

  matching_definitions AS (
    SELECT
      dar.definition_id,
      d.fk_entry_id,
      d.fk_source_id,
      d.definition,
      d.label,
      RANK
    FROM definitions_and_ranks AS dar
      JOIN definitions AS d
        ON dar.definition_id = d.definition_id
  ),

  matching_definition_groups AS (
    SELECT fk_entry_id,
      CASE sourceshortname
        WHEN 'ABY' THEN AVG(md.rank) * 3
        WHEN 'CCY' THEN AVG(md.rank) * 3
        WHEN 'WHK' THEN AVG(md.rank) * 3
        WHEN 'YF' THEN AVG(md.rank) * 3
        ELSE AVG(md.rank)
      END AS RANK,
      json_object('source', sourcename,
                  'definitions',
                  json_group_array(json_object(
                    'definition', definition))) AS definitions
    FROM matching_definitions AS md
    LEFT JOIN sources ON sources.source_id = md.fk_source_id
    GROUP BY fk_entry_id, fk_source_id
  ),

  matching_entries AS (
    SELECT
      entry_id,
      simplified,
      traditional,
      jyutping,
      pinyin,
      SUM(RANK) AS RANK,
      json_group_array(json(definitions)) AS definitions
    FROM matching_definition_groups AS mdg
    LEFT JOIN entries ON entries.entry_id = mdg.fk_entry_id
    GROUP BY entry_id
    ORDER BY RANK ASC, frequency DESC
  )

  SELECT entry_id, traditional, simplified, jyutping, pinyin, definitions FROM
    matching_entries
  """

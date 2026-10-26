//
//  Queries.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-21.
//

nonisolated let GLOB_OPERATOR = "GLOB"
nonisolated let REGEXP_OPERATOR = "REGEXP"

nonisolated let getEntryByRowId: String = """
      WITH matching_entry_ids AS (
        SELECT rowid FROM entries WHERE rowid = ?
      ),

      matching_definition_ids AS (
        SELECT definition_id, definition FROM definitions WHERE fk_entry_id
        IN matching_entry_ids
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

nonisolated let searchExamplesQuery: String = """
    WITH
      matching_example_ids AS (
        SELECT example_id
        FROM examples 
        WHERE traditional LIKE ? ESCAPE '\\'
      ),
      translations_with_source AS (
        SELECT
          s.sourcename AS source,
          mei.example_id AS example_id,
          json_group_array(
            DISTINCT json_object(
              'translation',
              translation,
              'language',
              language,
              'direct',
              direct
            )
          ) AS translation
        FROM
          matching_example_ids AS mei
          LEFT JOIN example_links AS el
            ON mei.example_id = el.fk_example_id
          LEFT JOIN example_translations AS et
            ON et.example_translation_id = el.fk_example_translation_id
          LEFT JOIN sources AS s
            ON s.source_id = el.fk_source_id
        GROUP BY s.sourcename, mei.example_id
      ),
      matching_translations AS (
        SELECT
          example_id,
          json_group_array(
            json_object(
              'source',
              source,
              'translations',
              json(translation)
            )
          ) AS translations
        FROM translations_with_source AS tws
        GROUP BY example_id
      ),
      matching_examples AS (
        SELECT
          example_id,
          traditional,
          simplified,
          pinyin,
          jyutping,
          language
        FROM examples AS e
        WHERE
          example_id IN (
            SELECT example_id
            FROM matching_example_ids
          )
      ),
      matching_examples_with_translations AS (
        SELECT
          max(sourcename) AS sourcename,
          traditional,
          simplified,
          pinyin,
          jyutping,
          language,
          translations
        FROM
          matching_examples AS me
          LEFT JOIN matching_translations AS mt
            ON me.example_id = mt.example_id
          LEFT JOIN definitions_examples_links AS del
            ON me.example_id = del.fk_example_id
          LEFT JOIN definitions AS d
            ON del.fk_definition_id = d.definition_id
          LEFT JOIN sources AS s ON d.fk_source_id = s.source_id
        GROUP BY
          traditional,
          simplified,
          pinyin,
          jyutping,
          language,
          translations
        ORDER BY me.example_id
      )
    SELECT
      sourcename,
      traditional,
      simplified,
      pinyin,
      jyutping,
      language,
      translations
    FROM matching_examples_with_translations
  """

from database import database

import sqlite3
import sys

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print(
            "Usage: python3 script.py <output database name> <database 1 filename> <database 2 filename>"
        )
        print("e.g. python3 script.py dict-merged.db dict.db dict-fr.db")
        sys.exit(1)

    db = sqlite3.connect(sys.argv[1])
    c = db.cursor()

    # Set version of database
    database.write_database_version(c)

    # Delete old tables and indices
    database.drop_tables(c)

    # Create new tables
    database.create_tables(c)

    # Attach new databases
    c.execute("ATTACH DATABASE '{}' AS db1".format(sys.argv[2]))
    c.execute("ATTACH DATABASE '{}' AS db2".format(sys.argv[3]))

    # Insert from first database
    c.execute(
        """INSERT INTO entries(traditional,
                simplified,
                pinyin,
                jyutping,
                frequency)
            SELECT traditional,
                simplified,
                pinyin,
                jyutping,
                frequency
            FROM db1.entries"""
    )
    c.execute(
        """INSERT INTO sources(sourcename,
                sourceshortname,
                version,
                description,
                legal,
                link,
                update_url,
                other)
            SELECT sourcename,
                sourceshortname,
                version,
                description,
                legal,
                link,
                update_url,
                other
            FROM db1.sources"""
    )
    c.execute(
        """INSERT INTO definitions(definition,
                label,
                fk_entry_id,
                fk_source_id)
            SELECT definition,
                label,
                fk_entry_id,
                fk_source_id
            FROM db1.definitions"""
    )
    c.execute(
        """INSERT INTO examples(example_id,
                traditional,
                simplified,
                pinyin,
                jyutping,
                language)
            SELECT example_id,
                traditional,
                simplified,
                pinyin,
                jyutping,
                language
            FROM db1.examples"""
    )
    c.execute(
        """INSERT INTO example_translations(example_translation_id,
                translation,
                language)
            SELECT example_translation_id,
                translation,
                language
            FROM db1.example_translations"""
    )
    c.execute(
        """INSERT INTO example_links(fk_example_id,
                fk_example_translation_id,
                fk_source_id,
                direct)
            SELECT fk_example_id,
                fk_example_translation_id,
                fk_source_id,
                direct
            FROM db1.example_links"""
    )
    c.execute(
        """INSERT INTO definitions_examples_links(fk_definition_id,
                fk_example_id)
            SELECT fk_definition_id,
                fk_example_id
            FROM db1.definitions_examples_links"""
    )

    # Insert from second database
    c.execute(
        """INSERT INTO entries(traditional, 
                simplified, 
                pinyin, 
                jyutping, 
                frequency)
            SELECT traditional,
                simplified,
                pinyin,
                jyutping, 
                frequency 
            FROM db2.entries"""
    )
    c.execute(
        """INSERT INTO sources(sourcename,
                sourceshortname,
                version,
                description,
                legal,
                link,
                update_url,
                other)
            SELECT sourcename,
                sourceshortname,
                version,
                description,
                legal,
                link,
                update_url,
                other
            FROM db2.sources"""
    )
    c.execute(
        """INSERT INTO examples(example_id,
                traditional,
                simplified,
                pinyin,
                jyutping,
                language)
            SELECT example_id,
                traditional,
                simplified,
                pinyin,
                jyutping,
                language
            FROM db2.examples"""
    )
    c.execute(
        """INSERT INTO example_translations(example_translation_id,
                translation,
                language)
            SELECT example_translation_id,
                translation,
                language
            FROM db2.example_translations"""
    )

    # Insert definitions separately, as their foreign key references need to be re-written
    c.execute(
        """WITH definitions_tmp AS (
                    SELECT entries.traditional AS traditional,
                        entries.simplified AS simplified,
                        entries.pinyin AS pinyin,
                        entries.jyutping AS jyutping,
                        sources.sourcename AS sourcename,
                        definitions.definition AS definition,
                        definitions.label AS label
                    FROM db2.entries, db2.definitions, db2.sources
                    WHERE db2.definitions.fk_entry_id = db2.entries.entry_id
                        AND db2.definitions.fk_source_id = db2.sources.source_id
            )

        INSERT INTO definitions(definition,
                label,
                fk_entry_id,
                fk_source_id)
            SELECT d.definition, d.label, e.entry_id, s.source_id
            FROM definitions_tmp AS d, sources AS s, entries AS e
            WHERE d.sourcename = s.sourcename
                AND d.traditional = e.traditional
                AND d.simplified = e.simplified
                AND d.pinyin = e.pinyin
                AND d.jyutping = e.jyutping
        """
    )

    # Insert example links separately, as their example and source foreign keys need to be rewritten
    c.execute(
        """WITH example_links_with_source AS (
                    SELECT example_links.fk_example_id AS fk_ei,
                        example_links.fk_example_translation_id AS fk_eti,
                        example_links.direct AS direct,
                        sources.sourcename AS sourcename
                    FROM db2.example_links, db2.sources
                    WHERE example_links.fk_source_id = db2.sources.source_id
            ),

            example_links_with_foreign_key AS (
                    SELECT traditional,
                        simplified,
                        pinyin,
                        jyutping,
                        language,
                        fk_eti,
                        direct,
                        sourcename
                    FROM example_links_with_source as elws,
                        db2.examples AS e
                    WHERE elws.fk_ei = e.example_id
            )

        INSERT INTO example_links(fk_example_id,
                fk_example_translation_id,
                fk_source_id,
                direct)
            SELECT e.example_id,
                elwfk.fk_eti,
                s.source_id,
                elwfk.direct
            FROM example_links_with_foreign_key AS elwfk,
                sources AS s,
                examples AS e
            WHERE s.sourcename = elwfk.sourcename
                AND e.traditional = elwfk.traditional
                AND e.simplified = elwfk.simplified
                AND e.pinyin = elwfk.pinyin
                AND e.jyutping = elwfk.jyutping
                AND e.language = elwfk.language
        """
    )

    # Insert definitions => example links

    # entry_and_definitions: [traditional | simplified | pinyin | jyutping | definition | label | source]
    # for each definition in db2, since this uniquely identifies the definition

    # Match that up with examples, so that we get defs_e_links_tmp:
    #      [example/entry traditional | example/entry simplified | example/entry pinyin | example/entry jyutping |
    #       definition_id | definition_label | definition | definition_source | sentence_language]
    # for each example in db2, since this uniquely identifies the example

    # In current database, get new_entry_and_definitions: [traditional | simplified | pinyin | jyutping | definition | label | source]

    # And replace the fk_definition_id for each example link when traditional/simplified/pinyin/jyutping/definition/label/source all match for a sentence.
    c.execute(
        """WITH entry_and_definitions AS (
                    SELECT entries.traditional AS traditional,
                        entries.simplified AS simplified,
                        entries.pinyin AS pinyin,
                        entries.jyutping AS jyutping,
                        definitions.definition AS definition,
                        definitions.label AS label,
                        definitions.definition_id AS definition_id,
                        sources.sourcename AS source
                    FROM db2.entries,
                        db2.definitions,
                        db2.sources
                    WHERE db2.definitions.fk_entry_id = db2.entries.entry_id
                        AND db2.definitions.fk_source_id = db2.sources.source_id
            ),

            defs_e_links_tmp AS (
                    SELECT
                        e.traditional AS example_traditional,
                        e.simplified AS example_simplified,
                        e.pinyin AS example_pinyin,
                        e.jyutping AS example_jyutping,
                        e.language AS example_language,
                        ed.definition AS definition,
                        ed.label AS label,
                        ed.traditional AS traditional,
                        ed.simplified AS simplified,
                        ed.pinyin AS pinyin,
                        ed.jyutping AS jyutping,
                        ed.source AS source
                    FROM db2.definitions_examples_links AS del,
                        db2.examples AS e,
                        entry_and_definitions AS ed
                    WHERE del.fk_definition_id = ed.definition_id
                        AND del.fk_example_id = e.example_id
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

            INSERT INTO definitions_examples_links(fk_definition_id,
                        fk_example_id)
                    SELECT ned.definition_id,
                        e.example_id
                    FROM defs_e_links_tmp AS del, new_entry_and_definitions AS ned,
                        examples AS e
                    WHERE del.example_traditional = e.traditional
                        AND del.example_simplified = e.simplified
                        AND del.example_pinyin = e.pinyin
                        AND del.example_jyutping = e.jyutping
                        AND del.example_language = e.language
                        AND del.definition = ned.definition
                        AND del.label = ned.label
                        AND del.traditional = ned.traditional
                        AND del.simplified = ned.simplified
                        AND del.pinyin = ned.pinyin
                        AND del.jyutping = ned.jyutping
                        AND del.source = ned.source
        """
    )

    # Populate FTS versions of tables
    database.generate_indices(c)

    db.commit()
    db.close()

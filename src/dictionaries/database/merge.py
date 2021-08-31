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
        """INSERT INTO entries(traditional, simplified, pinyin, jyutping, frequency)
            SELECT traditional, simplified, pinyin, jyutping, frequency FROM db1.entries"""
    )
    c.execute(
        """INSERT INTO sources(sourcename, sourceshortname, version, description, legal, link, update_url, other)
        SELECT sourcename, sourceshortname, version, description, legal, link, update_url, other FROM db1.sources"""
    )
    c.execute(
        """INSERT INTO definitions(definition, fk_entry_id, fk_source_id)
        SELECT definition, fk_entry_id, fk_source_id FROM db1.definitions"""
    )
    c.execute(
        """INSERT INTO chinese_sentences(chinese_sentence_id, traditional, simplified, pinyin, jyutping, language)
        SELECT chinese_sentence_id, traditional, simplified, pinyin, jyutping, language FROM db1.chinese_sentences"""
    )
    c.execute(
        """INSERT INTO nonchinese_sentences(non_chinese_sentence_id, sentence, language)
        SELECT non_chinese_sentence_id, sentence, language FROM db1.nonchinese_sentences"""
    )
    c.execute(
        """INSERT INTO sentence_links(fk_chinese_sentence_id, fk_non_chinese_sentence_id, fk_source_id, direct)
        SELECT fk_chinese_sentence_id, fk_non_chinese_sentence_id, fk_source_id, direct FROM db1.sentence_links"""
    )
    c.execute(
        """INSERT INTO definitions_chinese_sentences_links(fk_definition_id, fk_chinese_sentence_id)
        SELECT fk_definition_id, fk_chinese_sentence_id FROM db1.definitions_chinese_sentences_links"""
    )

    # Insert from second database
    c.execute(
        """INSERT INTO entries(traditional, simplified, pinyin, jyutping, frequency)
        SELECT traditional, simplified, pinyin, jyutping, frequency FROM db2.entries"""
    )
    c.execute(
        """INSERT INTO sources(sourcename, sourceshortname, version, description, legal, link, update_url, other)
        SELECT sourcename, sourceshortname, version, description, legal, link, update_url, other FROM db2.sources"""
    )
    c.execute(
        """INSERT INTO chinese_sentences(chinese_sentence_id, traditional, simplified, pinyin, jyutping, language)
        SELECT chinese_sentence_id, traditional, simplified, pinyin, jyutping, language FROM db2.chinese_sentences"""
    )
    c.execute(
        """INSERT INTO nonchinese_sentences(non_chinese_sentence_id, sentence, language)
        SELECT non_chinese_sentence_id, sentence, language FROM db2.nonchinese_sentences"""
    )

    # Insert definitions separately, as their foreign key references need to be re-written
    c.execute(
        """WITH definitions_tmp AS (
                    SELECT entries.traditional AS traditional, entries.simplified AS simplified, entries.pinyin AS pinyin, entries.jyutping AS jyutping,
                        sources.sourcename AS sourcename, definitions.definition AS definition, definitions.label AS label
                    FROM db2.entries, db2.definitions, db2.sources
                    WHERE db2.definitions.fk_entry_id = db2.entries.entry_id AND db2.definitions.fk_source_id = db2.sources.source_id
            )

        INSERT INTO definitions(definition, label, fk_entry_id, fk_source_id)
            SELECT d.definition, d.label, e.entry_id, s.source_id
            FROM definitions_tmp AS d, sources AS s, entries AS e
            WHERE d.sourcename = s.sourcename
                AND d.traditional = e.traditional
                AND d.simplified = e.simplified
                AND d.pinyin = e.pinyin
                AND d.jyutping = e.jyutping
        """
    )

    # Insert sentence links separate, as their source foreign key needs to be rewritten
    c.execute(
        """WITH sentence_links_tmp AS (
                    SELECT sentence_links.fk_chinese_sentence_id AS fk_csi,
                        sentence_links.fk_non_chinese_sentence_id AS fk_ncsi,
                        sentence_links.direct AS direct,
                        sources.sourcename AS sourcename
                    FROM db2.sentence_links, db2.sources
                    WHERE sentence_links.fk_source_id = db2.sources.source_id
            )

        INSERT INTO sentence_links(fk_chinese_sentence_id, fk_non_chinese_sentence_id, fk_source_id, direct)
            SELECT sl.fk_csi, sl.fk_ncsi, s.source_id, sl.direct
            FROM sentence_links_tmp AS sl, sources AS s
            WHERE sl.sourcename = s.sourcename
        """
    )

    # Insert definitions => sentence links

    # entry_and_definitions: [traditional | simplified | pinyin | jyutping | definition]
    # for each definition in db2, since this uniquely identifies the definition

    # Match that up with sentences, so that we get dfs_s_links_tmp: [traditional | simplified | pinyin | jyutping | definition | sentence]
    # for each sentence in db2, since this uniquely identifies the sentence

    # In current database, get new_entry_and_defnitions: [traditional | simplified | pinyin | jyutping | definition]

    # And replace the fk_definition_id for each sentence link when traditional/simplified/pinyin/jyutping/definition all match for a sentence.
    c.execute(
        """WITH entry_and_definitions AS (
                    SELECT entries.traditional AS traditional, entries.simplified AS simplified, entries.pinyin AS pinyin, entries.jyutping AS jyutping,
                        definitions.definition AS definition, definitions.definition_id AS definition_id
                    FROM db2.entries, db2.definitions
                    WHERE db2.definitions.fk_entry_id = db2.entries.entry_id
            ),

            defs_s_links_tmp AS (
                    SELECT dsl.fk_definition_id AS fdi,
                        dsl.fk_chinese_sentence_id AS fcsi,
                        ed.definition AS definition,
                        ed.traditional AS traditional,
                        ed.simplified AS simplified,
                        ed.pinyin AS pinyin,
                        ed.jyutping AS jyutping
                    FROM db2.definitions_chinese_sentences_links AS dsl, entry_and_definitions AS ed
                    WHERE dsl.fk_definition_id = ed.definition_id
            ),

            new_entry_and_definitions AS (
                    SELECT entries.traditional AS traditional, entries.simplified AS simplified, entries.pinyin AS pinyin, entries.jyutping AS jyutping,
                        definitions.definition AS definition, definitions.definition_id AS definition_id
                    FROM entries, definitions
                    WHERE definitions.fk_entry_id = entries.entry_id
            )

            INSERT INTO definitions_chinese_sentences_links(fk_definition_id, fk_chinese_sentence_id)
                    SELECT ned.definition_id,
                        dsl.fcsi
                    FROM defs_s_links_tmp AS dsl, new_entry_and_definitions AS ned
                    WHERE dsl.definition = ned.definition
                        AND dsl.traditional = ned.traditional
                        AND dsl.simplified = ned.simplified
                        AND dsl.pinyin = ned.pinyin
                        AND dsl.jyutping = ned.jyutping
        """
    )

    # Populate FTS versions of tables
    database.generate_indices(c)

    db.commit()
    db.close()

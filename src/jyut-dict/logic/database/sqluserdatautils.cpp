#include "sqluserdatautils.h"

#include "logic/database/queryparseutils.h"
#include "logic/database/sqldatabasemanager.h"
#include "logic/entry/entry.h"

#include <QSQLError>
#include <QSqlQuery>
#include <QtConcurrent/QtConcurrent>

SQLUserDataUtils::SQLUserDataUtils(std::shared_ptr<SQLDatabaseManager> manager)
    : _manager{manager}
{
}

void SQLUserDataUtils::registerObserver(ISearchObserver *observer)
{
    std::lock_guard<std::mutex> notifyLock{_notifyMutex};
    _observers.push_back(observer);
}

void SQLUserDataUtils::deregisterObserver(ISearchObserver *observer)
{
    std::lock_guard<std::mutex> notifyLock{_notifyMutex};
    _observers.remove(observer);
}

void SQLUserDataUtils::notifyObservers(const std::vector<Entry> &results,
                                       bool emptyQuery)
{
    std::lock_guard<std::mutex> notifyLock{_notifyMutex};
    std::list<ISearchObserver *>::const_iterator it = _observers.begin();
    while (it != _observers.end()) {
        (static_cast<ISearchObserver *>(*it))->callback(results, emptyQuery);
        ++it;
    }
}

void SQLUserDataUtils::notifyObservers(bool entryExists, const Entry &entry)
{
    std::lock_guard<std::mutex> notifyLock{_notifyMutex};
    std::list<ISearchObserver *>::const_iterator it = _observers.begin();
    while (it != _observers.end()) {
        (static_cast<ISearchObserver *>(*it))->callback(entryExists, entry);
        ++it;
    }
}

void SQLUserDataUtils::searchForAllFavouritedWords(void)
{
    if (!_manager) {
        std::cout << "No database specified!" << std::endl;
        return;
    }
    std::ignore = QtConcurrent::run(&SQLUserDataUtils::searchForAllFavouritedWordsThread,
                                    this);
}

void SQLUserDataUtils::checkIfEntryHasBeenFavourited(const Entry &entry)
{
    if (!_manager) {
        std::cout << "No database specified!" << std::endl;
        return;
    }
    std::ignore = QtConcurrent::run(&SQLUserDataUtils::checkIfEntryHasBeenFavouritedThread,
                                    this,
                                    entry);
}

void SQLUserDataUtils::favouriteEntry(const Entry &entry)
{
    if (!_manager) {
        std::cout << "No database specified!" << std::endl;
        return;
    }
    std::ignore = QtConcurrent::run(&SQLUserDataUtils::favouriteEntryThread,
                                    this,
                                    entry);
}

void SQLUserDataUtils::unfavouriteEntry(const Entry &entry)
{
    if (!_manager) {
        std::cout << "No database specified!" << std::endl;
        return;
    }
    std::ignore = QtConcurrent::run(&SQLUserDataUtils::unfavouriteEntryThread,
                                    this,
                                    entry);
}

// NOTE: If you are modifying this, you may also want to modify
// the search functions in SQLSearch.cpp as well!
void SQLUserDataUtils::searchForAllFavouritedWordsThread(void)
{
    std::vector<Entry> results;

    QSqlQuery query{_manager->getDatabase()};

    query.exec(
        //// Get list of entry ids that are in the user's favourites
        //// This CTE is used multiple times; would be nice if could materialize it
        "WITH matching_entry_ids AS ( "
        "  SELECT entry_id, timestamp from entries "
        "  INNER JOIN user.favourite_words "
        "    ON entries.simplified = favourite_words.simplified "
        "    AND entries.traditional = favourite_words.traditional "
        "    AND entries.jyutping = favourite_words.jyutping "
        "    AND entries.pinyin = favourite_words.pinyin "
        "), "
        " "
        //// Get the list of all definitions for those entries
        //// This CTE is used multiple times; would be nice if could materialize it
        "matching_definition_ids AS ( "
        "  SELECT definition_id, definition FROM definitions WHERE fk_entry_id "
        "    IN ("
        "      SELECT entry_id FROM matching_entry_ids "
        "    )"
        "), "
        " "
        //// Get corresponding example ids for each of those definitions
        //// This CTE is used multiple times; would be nice if could materialize it
        "matching_example_ids AS ( "
        "  SELECT definition_id, fk_example_id "
        "  FROM matching_definition_ids AS mdi "
        "  JOIN definitions_examples_links AS del ON "
        "mdi.definition_id = del.fk_definition_id "
        "), "
        " "
        //// Get translations for each of the examples
        "matching_translations AS ( "
        "  SELECT mei.fk_example_id, "
        "    json_group_array(DISTINCT "
        "      json_object('translation', translation, "
        "                  'language', language, "
        "                  'direct', direct "
        "    )) AS translations "
        "  FROM matching_example_ids AS mei "
        "  JOIN example_links AS el ON mei.fk_example_id = "
        "el.fk_example_id "
        "  JOIN example_translations AS es ON es.example_translation_id = "
        "el.fk_example_translation_id "
        "  GROUP BY mei.fk_example_id "
        "), "
        " "
        //// Get example data for each of the example ids
        "matching_examples AS ( "
        " SELECT example_id, traditional, simplified, pinyin, "
        "   jyutping, language "
        " FROM examples AS e "
        " WHERE example_id IN ( "
        "   SELECT fk_example_id FROM matching_example_ids "
        " ) "
        "),"
        " "
        //// Get translations for each of those examples
        "matching_examples_with_translations AS ( "
        "  SELECT example_id, "
        "    json_object('traditional', traditional, "
        "                'simplified', simplified, "
        "                'pinyin', pinyin, "
        "                'jyutping', jyutping, "
        "                'language', language, "
        "                'translations', json(translations)) AS example "
        "  FROM matching_examples AS me "
        "  LEFT JOIN matching_translations AS mt ON me.example_id = "
        "mt.fk_example_id "
        "), "
        " "
        //// Get definition data for each matching definition
        "matching_definitions AS ( "
        "  SELECT definition_id, fk_entry_id, fk_source_id, definition, "
        "    label "
        "  FROM definitions "
        "  WHERE definitions.definition_id IN ( "
        "    SELECT definition_id FROM matching_definition_ids"
        "  ) "
        "), "
        " "
        //// Create definition object with examples for each definition
        "matching_definitions_with_examples AS ( "
        "  SELECT fk_entry_id, fk_source_id, "
        "    json_object('definition', definition, "
        "                'label', label, 'examples', "
        "                json_group_array(json(example))) AS definition "
        "  FROM matching_definitions AS md "
        "  LEFT JOIN matching_example_ids AS mei ON md.definition_id "
        "= mei.definition_id "
        "  LEFT JOIN matching_examples_with_translations AS mewt ON "
        "mei.fk_example_id = mewt.example_id "
        "  GROUP BY md.definition_id "
        "), "
        " "
        //// Create definition groups for definitions of the same entry that come from the same source
        "matching_definition_groups AS ( "
        "  SELECT fk_entry_id, "
        "    json_object('source', sourcename, "
        "                'definitions', "
        "                json_group_array(json(definition))) AS definitions "
        "  FROM matching_definitions_with_examples AS mdwe "
        "  LEFT JOIN sources ON sources.source_id = mdwe.fk_source_id "
        "  GROUP BY fk_entry_id, fk_source_id "
        "), "
        " "
        //// Construct the final entry object
        "matching_entries AS ( "
        "  SELECT simplified, traditional, jyutping, pinyin, "
        "json_group_array(json(definitions)) AS definitions "
        "  FROM matching_definition_groups AS mdg "
        "  LEFT JOIN entries ON entries.entry_id = mdg.fk_entry_id "
        "  GROUP BY entry_id "
        ") "
        " "
        "SELECT "
        "  e.simplified, e.traditional, e.jyutping, e.pinyin, e.definitions "
        "FROM "
        "  matching_entries AS e "
        "INNER JOIN user.favourite_words "
        "  ON e.simplified = favourite_words.simplified "
        "  AND e.traditional = favourite_words.traditional "
        "  AND e.jyutping = favourite_words.jyutping "
        "  AND e.pinyin = favourite_words.pinyin "
        "ORDER BY timestamp ASC ");

    results = QueryParseUtils::parseEntries(query);
    _manager->closeAndRemoveDatabaseConnection();

    notifyObservers(results, /*emptyQuery=*/false);
}

void SQLUserDataUtils::checkIfEntryHasBeenFavouritedThread(const Entry &entry)
{
    bool existence = false;
    QSqlQuery query{_manager->getDatabase()};

    query.prepare(
        "SELECT EXISTS (SELECT 1 FROM user.favourite_words WHERE "
        "traditional=? AND simplified=? AND jyutping=? AND pinyin=?) "
        "AS existence");
    query.addBindValue(entry.getTraditional().c_str());
    query.addBindValue(entry.getSimplified().c_str());
    query.addBindValue(entry.getJyutping().c_str());
    query.addBindValue(entry.getPinyin().c_str());
    query.exec();
    existence = QueryParseUtils::parseExistence(query);
    _manager->closeAndRemoveDatabaseConnection();

    notifyObservers(existence, entry);
}

void SQLUserDataUtils::favouriteEntryThread(const Entry &entry)
{
    QSqlQuery query{_manager->getDatabase()};

    query.prepare(
        "INSERT INTO user.favourite_words(traditional, simplified, "
        " jyutping, pinyin, fk_list_id, timestamp) "
        "values(?, ?, ?, ?, 1, datetime(\"now\")) ");
    query.addBindValue(entry.getTraditional().c_str());
    query.addBindValue(entry.getSimplified().c_str());
    query.addBindValue(entry.getJyutping().c_str());
    query.addBindValue(entry.getPinyin().c_str());
    query.exec();
    query.exec("COMMIT");
    _manager->closeAndRemoveDatabaseConnection();

    checkIfEntryHasBeenFavourited(entry);
    searchForAllFavouritedWords();
}

void SQLUserDataUtils::unfavouriteEntryThread(const Entry &entry)
{
    QSqlQuery query{_manager->getDatabase()};

    query.prepare(
        "DELETE FROM user.favourite_words WHERE "
        "traditional=? AND simplified=? AND jyutping=? AND pinyin=?");
    query.addBindValue(entry.getTraditional().c_str());
    query.addBindValue(entry.getSimplified().c_str());
    query.addBindValue(entry.getJyutping().c_str());
    query.addBindValue(entry.getPinyin().c_str());
    query.exec();
    query.exec("COMMIT");
    _manager->closeAndRemoveDatabaseConnection();

    checkIfEntryHasBeenFavourited(entry);
    searchForAllFavouritedWords();
}

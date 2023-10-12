#include "sqlsearch.h"

#include "logic/database/queryparseutils.h"
#include "logic/utils/chineseutils.h"
#include "logic/utils/utils.h"

#include <QtConcurrent/QtConcurrent>

#ifdef Q_OS_WIN
#include <cctype>
#endif

SQLSearch::SQLSearch()
{
    std::random_device rd;
    _generator = std::mt19937_64{rd()};
}

SQLSearch::SQLSearch(std::shared_ptr<SQLDatabaseManager> manager)
    : _manager{manager}
{
    std::random_device rd;
    _generator = std::mt19937_64{rd()};
}

SQLSearch::~SQLSearch()
{
    foreach (auto future, _futures) {
        future.waitForFinished();
    }
}

void SQLSearch::registerObserver(ISearchObserver *observer)
{
    std::lock_guard<std::mutex> notifyLock{_notifyMutex};
    _observers.push_back(observer);
}

void SQLSearch::deregisterObserver(ISearchObserver *observer)
{
    std::lock_guard<std::mutex> notifyLock{_notifyMutex};
    _observers.remove(observer);
}

// Do not call this function without first acquiring the _notifyMutex!
void SQLSearch::notifyObservers(const std::vector<Entry> &results, bool emptyQuery)
{
    std::list<ISearchObserver *>::const_iterator it = _observers.begin();
    while (it != _observers.end()) {
        (static_cast<ISearchObserver *>(*it))->callback(results, emptyQuery);
        ++it;
    }
}

// Do not call this function without first acquiring the _notifyMutex!
void SQLSearch::notifyObservers(const std::vector<SourceSentence> &results,
                                bool emptyQuery)
{
    std::list<ISearchObserver *>::const_iterator it = _observers.begin();
    while (it != _observers.end()) {
        (static_cast<ISearchObserver *>(*it))->callback(results, emptyQuery);
        ++it;
    }
}

// This version assumes an empty result set
void SQLSearch::notifyObserversOfEmptySet(bool emptyQuery,
                                          const unsigned long long queryID)
{
    std::lock_guard<std::mutex> notifyLock{_notifyMutex};
    if (queryID != _queryID) {
        return;
    }

    std::vector<Entry> results;
    notifyObservers(results, emptyQuery);
}

void SQLSearch::notifyObserversIfQueryIdCurrent(const std::vector<Entry> &results,
                                                bool emptyQuery,
                                                const unsigned long long queryID)
{
    std::lock_guard<std::mutex> notifyLock{_notifyMutex};
    if (queryID != _queryID) {
        return;
    }

    notifyObservers(results, emptyQuery);
}

void SQLSearch::notifyObserversIfQueryIdCurrent(const std::vector<SourceSentence> &results,
                                                bool emptyQuery,
                                                const unsigned long long queryID)
{
    std::lock_guard<std::mutex> notifyLock{_notifyMutex};
    if (queryID != _queryID) {
        return;
    }

    notifyObservers(results, emptyQuery);
}

unsigned long long SQLSearch::generateAndSetQueryID(void) {
    unsigned long long queryID = _dist(_generator);
    _queryID = queryID;
    return queryID;
}

bool SQLSearch::checkQueryIDCurrent(const unsigned long long queryID) const {
    if (queryID != _queryID) {
        return false;
    }
    return true;
}

void SQLSearch::searchSimplified(const QString &searchTerm)
{
    unsigned long long queryID = generateAndSetQueryID();
    runThread(&SQLSearch::searchSimplifiedThread, searchTerm, queryID);
}

void SQLSearch::searchTraditional(const QString &searchTerm)
{
    unsigned long long queryID = generateAndSetQueryID();
    runThread(&SQLSearch::searchTraditionalThread, searchTerm, queryID);
}

void SQLSearch::searchJyutping(const QString &searchTerm)
{
    unsigned long long queryID = generateAndSetQueryID();
    runThread(&SQLSearch::searchJyutpingThread, searchTerm, queryID);
}

void SQLSearch::searchPinyin(const QString &searchTerm)
{
    unsigned long long queryID = generateAndSetQueryID();
    runThread(&SQLSearch::searchPinyinThread, searchTerm, queryID);
}

void SQLSearch::searchEnglish(const QString &searchTerm)
{
    unsigned long long queryID = generateAndSetQueryID();
    runThread(&SQLSearch::searchEnglishThread, searchTerm, queryID);
}

void SQLSearch::searchByUnique(const QString &simplified,
                               const QString &traditional,
                               const QString &jyutping,
                               const QString &pinyin)
{
    if (!_manager) {
        std::cout << "No database specified!" << std::endl;
        return;
    }

    unsigned long long queryID = generateAndSetQueryID();
    auto future = QtConcurrent::run(this,
                                    &SQLSearch::searchByUniqueThread,
                                    simplified,
                                    traditional,
                                    jyutping,
                                    pinyin,
                                    queryID);
    _futures.append(future);
}

void SQLSearch::searchTraditionalSentences(const QString &searchTerm)
{
    unsigned long long queryID = generateAndSetQueryID();
    runThread(&SQLSearch::searchTraditionalSentencesThread, searchTerm, queryID);
}

void SQLSearch::runThread(void (SQLSearch::*threadFunction)(const QString &searchTerm, const unsigned long long queryID),
                          const QString &searchTerm, const unsigned long long queryID)
{
    if (searchTerm.isEmpty()) {
        notifyObserversOfEmptySet(true, queryID);
        return;
    }

    if (!_manager) {
        std::cout << "No database specified!" << std::endl;
        return;
    }

    auto future = QtConcurrent::run(this, threadFunction, searchTerm, queryID);
    _futures.append(future);
}

// NOTE: If you are modifying these functions, you may also want to modify
// the search functions in SQLUserDataUtils.cpp as well!

// For searching simplified and traditional, we use GLOB, so that wildcard
// characters like * and ? can be used.
void SQLSearch::searchSimplifiedThread(const QString &searchTerm,
                                       const unsigned long long queryID)
{
    // When the search term is surrounded by quotes, search for only term
    // inside quotes (not the quotes themselves)
    bool searchExactMatch = ((searchTerm.at(0) == "\""
                              && searchTerm.at(searchTerm.size() - 1) == "\"")
                             || (searchTerm.startsWith("”")
                                 && searchTerm.endsWith("“")))
                            && searchTerm.length() >= 3;
    bool dontAppendWildcard = searchTerm.at(searchTerm.size() - 1) == "$";

    QString searchTermWithoutQuotes;
    QString searchTermWithoutEndPositionMarker;
    if (searchExactMatch) {
        searchTermWithoutQuotes = searchTerm.mid(1, searchTerm.size() - 2);
    } else if (dontAppendWildcard) {
        searchTermWithoutEndPositionMarker = searchTerm.chopped(1);
    }

    std::vector<Entry> results;

    QSqlQuery query{_manager->getDatabase()};
    query.prepare(
        //// Get list of entry ids whose simplified form matches the query
        "WITH matching_entry_ids AS ( "
        "  SELECT rowid FROM entries WHERE simplified GLOB ?"
        "), "
        " "
        //// Get the list of all definitions for those entries
        //// This CTE is used multiple times; would be nice if could materialize it
        "matching_definition_ids AS ( "
        "  SELECT definition_id, definition FROM definitions WHERE fk_entry_id "
        "  IN matching_entry_ids "
        "), "
        " "
        //// Get corresponding sentence ids for each of those definitions
        //// This CTE is used multiple times; would be nice if could materialize it
        "matching_chinese_sentence_ids AS ( "
        "  SELECT definition_id, fk_chinese_sentence_id "
        "  FROM matching_definition_ids AS mdi "
        "  JOIN definitions_chinese_sentences_links AS dcsl ON "
        "    mdi.definition_id = dcsl.fk_definition_id "
        "), "
        " "
        //// Get translations for each of the sentences
        "matching_translations AS ( "
        "  SELECT mcsi.fk_chinese_sentence_id, "
        "    json_group_array(DISTINCT "
        "      json_object('sentence', sentence, "
        "                  'language', language, "
        "                  'direct', direct "
        "    )) AS translation "
        "  FROM matching_chinese_sentence_ids AS mcsi "
        "  JOIN sentence_links AS sl ON mcsi.fk_chinese_sentence_id = "
        "    sl.fk_chinese_sentence_id "
        "  JOIN nonchinese_sentences AS ncs ON ncs.non_chinese_sentence_id = "
        "    sl.fk_non_chinese_sentence_id "
        "  GROUP BY mcsi.fk_chinese_sentence_id "
        "), "
        " "
        //// Get sentence data for each of the sentence ids
        "matching_sentences AS ( "
        " SELECT chinese_sentence_id, traditional, simplified, pinyin, "
        "   jyutping, language "
        " FROM chinese_sentences AS cs "
        " WHERE chinese_sentence_id IN ( "
        "   SELECT fk_chinese_sentence_id FROM "
        "     matching_chinese_sentence_ids "
        " ) "
        "),"
        " "
        //// Get translations for each of those sentences
        "matching_sentences_with_translations AS ( "
        "  SELECT chinese_sentence_id, "
        "    json_object('traditional', traditional, "
        "                'simplified', simplified, "
        "                'pinyin', pinyin, "
        "                'jyutping', jyutping, "
        "                'language', language, "
        "                'translations', json(translation)) AS sentence "
        "  FROM matching_sentences AS ms "
        "  LEFT JOIN matching_translations AS mt ON ms.chinese_sentence_id = "
        "    mt.fk_chinese_sentence_id "
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
        //// Create definition object with sentences for each definition
        "matching_definitions_with_sentences AS ( "
        "  SELECT fk_entry_id, fk_source_id, "
        "    json_object('definition', definition, "
        "                'label', label, 'sentences', "
        "                json_group_array(json(sentence))) AS definition "
        "  FROM matching_definitions AS md "
        "  LEFT JOIN matching_chinese_sentence_ids AS mcsi ON "
        "    md.definition_id = mcsi.definition_id "
        "  LEFT JOIN matching_sentences_with_translations AS mswt ON "
        "    mcsi.fk_chinese_sentence_id = mswt.chinese_sentence_id "
        "  GROUP BY md.definition_id "
        "), "
        " "
        //// Create definition groups for definitions of the same entry that come from the same source
        "matching_definition_groups AS ( "
        "  SELECT fk_entry_id, "
        "    json_object('source', sourcename, "
        "                'definitions', "
        "                json_group_array(json(definition))) AS definitions "
        "  FROM matching_definitions_with_sentences AS mdws "
        "  LEFT JOIN sources ON sources.source_id = mdws.fk_source_id "
        "  GROUP BY fk_entry_id, fk_source_id "
        "), "
        " "
        //// Construct the final entry object
        "matching_entries AS ( "
        "  SELECT simplified, traditional, jyutping, pinyin, "
        "    json_group_array(json(definitions)) AS definitions "
        "  FROM matching_definition_groups AS mdg "
        "  LEFT JOIN entries ON entries.entry_id = mdg.fk_entry_id "
        "  GROUP BY entry_id "
        "  ORDER BY frequency DESC "
        ") "
        " "
        "SELECT simplified, traditional, jyutping, pinyin, definitions FROM "
        "  matching_entries");
    if (searchExactMatch) {
        query.addBindValue(searchTermWithoutQuotes);
    } else if (dontAppendWildcard) {
        query.addBindValue(searchTermWithoutEndPositionMarker);
    } else {
        query.addBindValue(searchTerm + "*");
    }
    query.exec();

    // Do not parse results if new query has been made
    if (!checkQueryIDCurrent(queryID)) {
        return;
    }
    results = QueryParseUtils::parseEntries(query);

    if (!checkQueryIDCurrent(queryID)) {
        return;
    }
    notifyObserversIfQueryIdCurrent(results, /*emptyQuery=*/false, queryID);
}

void SQLSearch::searchTraditionalThread(const QString &searchTerm,
                                        const unsigned long long queryID)
{
    bool searchExactMatch = ((searchTerm.at(0) == "\""
                              && searchTerm.at(searchTerm.size() - 1) == "\"")
                             || (searchTerm.startsWith("“")
                                 && searchTerm.endsWith("”")))
                            && searchTerm.length() >= 3;
    bool dontAppendWildcard = searchTerm.at(searchTerm.size() - 1) == "$";

    QString searchTermWithoutQuotes;
    QString searchTermWithoutEndPositionMarker;
    if (searchExactMatch) {
        searchTermWithoutQuotes = searchTerm.mid(1, searchTerm.size() - 2);
    } else if (dontAppendWildcard) {
        searchTermWithoutEndPositionMarker = searchTerm.chopped(1);
    }

    std::vector<Entry> results;

    QSqlQuery query{_manager->getDatabase()};
    query.prepare(
        //// Get list of entry ids whose traditional form matches the query
        "WITH matching_entry_ids AS ( "
        "  SELECT rowid FROM entries WHERE traditional GLOB ?"
        "), "
        " "
        //// Get the list of all definitions for those entries
        //// This CTE is used multiple times; would be nice if could materialize it
        "matching_definition_ids AS ( "
        "  SELECT definition_id, definition FROM definitions WHERE fk_entry_id "
        "  IN matching_entry_ids "
        "), "
        " "
        //// Get corresponding sentence ids for each of those definitions
        //// This CTE is used multiple times; would be nice if could materialize it
        "matching_chinese_sentence_ids AS ( "
        "  SELECT definition_id, fk_chinese_sentence_id "
        "  FROM matching_definition_ids AS mdi "
        "  JOIN definitions_chinese_sentences_links AS dcsl ON "
        "    mdi.definition_id = dcsl.fk_definition_id "
        "), "
        " "
        //// Get translations for each of the sentences
        "matching_translations AS ( "
        "  SELECT mcsi.fk_chinese_sentence_id, "
        "    json_group_array(DISTINCT "
        "      json_object('sentence', sentence, "
        "                  'language', language, "
        "                  'direct', direct "
        "    )) AS translation "
        "  FROM matching_chinese_sentence_ids AS mcsi "
        "  JOIN sentence_links AS sl ON mcsi.fk_chinese_sentence_id = "
        "    sl.fk_chinese_sentence_id "
        "  JOIN nonchinese_sentences AS ncs ON ncs.non_chinese_sentence_id = "
        "    sl.fk_non_chinese_sentence_id "
        "  GROUP BY mcsi.fk_chinese_sentence_id "
        "), "
        " "
        //// Get sentence data for each of the sentence ids
        "matching_sentences AS ( "
        " SELECT chinese_sentence_id, traditional, simplified, pinyin, "
        "   jyutping, language "
        " FROM chinese_sentences AS cs "
        " WHERE chinese_sentence_id IN ( "
        "   SELECT fk_chinese_sentence_id FROM "
        "     matching_chinese_sentence_ids "
        " ) "
        "),"
        " "
        //// Get translations for each of those sentences
        "matching_sentences_with_translations AS ( "
        "  SELECT chinese_sentence_id, "
        "    json_object('traditional', traditional, "
        "                'simplified', simplified, "
        "                'pinyin', pinyin, "
        "                'jyutping', jyutping, "
        "                'language', language, "
        "                'translations', json(translation)) AS sentence "
        "  FROM matching_sentences AS ms "
        "  LEFT JOIN matching_translations AS mt ON ms.chinese_sentence_id = "
        "    mt.fk_chinese_sentence_id "
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
        //// Create definition object with sentences for each definition
        "matching_definitions_with_sentences AS ( "
        "  SELECT fk_entry_id, fk_source_id, "
        "    json_object('definition', definition, "
        "                'label', label, 'sentences', "
        "                json_group_array(json(sentence))) AS definition "
        "  FROM matching_definitions AS md "
        "  LEFT JOIN matching_chinese_sentence_ids AS mcsi ON "
        "    md.definition_id = mcsi.definition_id "
        "  LEFT JOIN matching_sentences_with_translations AS mswt ON "
        "    mcsi.fk_chinese_sentence_id = mswt.chinese_sentence_id "
        "  GROUP BY md.definition_id "
        "), "
        " "
        //// Create definition groups for definitions of the same entry that come from the same source
        "matching_definition_groups AS ( "
        "  SELECT fk_entry_id, "
        "    json_object('source', sourcename, "
        "                'definitions', "
        "                json_group_array(json(definition))) AS definitions "
        "  FROM matching_definitions_with_sentences AS mdws "
        "  LEFT JOIN sources ON sources.source_id = mdws.fk_source_id "
        "  GROUP BY fk_entry_id, fk_source_id "
        "), "
        " "
        //// Construct the final entry object
        "matching_entries AS ( "
        "  SELECT simplified, traditional, jyutping, pinyin, "
        "    json_group_array(json(definitions)) AS definitions "
        "  FROM matching_definition_groups AS mdg "
        "  LEFT JOIN entries ON entries.entry_id = mdg.fk_entry_id "
        "  GROUP BY entry_id "
        "  ORDER BY frequency DESC "
        ") "
        " "
        "SELECT simplified, traditional, jyutping, pinyin, definitions FROM "
        "  matching_entries");
    if (searchExactMatch) {
        query.addBindValue(searchTermWithoutQuotes);
    } else if (dontAppendWildcard) {
        query.addBindValue(searchTermWithoutEndPositionMarker);
    } else {
        query.addBindValue(searchTerm + "*");
    }
    query.exec();

    // Do not parse results if new query has been made
    if (!checkQueryIDCurrent(queryID)) {
        return;
    }
    results = QueryParseUtils::parseEntries(query);

    if (!checkQueryIDCurrent(queryID)) { return; }
    notifyObserversIfQueryIdCurrent(results, /*emptyQuery=*/false, queryID);
}

// For searching Jyutping and Pinyin, we use GLOB, so that wildcard characters
// like * and ? can be used.
//
// !NOTE! Using QSQLQuery's positional placeholder method automatically
// surrounds the bound value with single quotes, i.e. "'". There is no need
// to add another set of quotes around placeholder values.
void SQLSearch::searchJyutpingThread(const QString &searchTerm,
                                     const unsigned long long queryID)
{
    // When the search term is surrounded by quotes, search for only term
    // inside quotes (not the quotes themselves)
    // Unlike the simplified/traditional search, only trigger exact match
    // searching if enclosed by Western quotes (U+0022).
    bool searchExactMatch = searchTerm.at(0) == "\""
                            && searchTerm.at(searchTerm.size() - 1) == "\""
                            && searchTerm.length() >= 3;
    bool dontAppendWildcard = searchTerm.at(searchTerm.size() - 1) == "$";

    std::vector<std::string> jyutpingWords;
    if (searchExactMatch) {
        QString searchTermWithoutQuotes = searchTerm.mid(1,
                                                         searchTerm.size() - 2);
        Utils::split(searchTermWithoutQuotes.toStdString(), ' ', jyutpingWords);
    } else {
        jyutpingWords
            = ChineseUtils::segmentJyutping(searchTerm,
                                            /* removeSpecialCharacters */ true,
                                            /* removeGlobCharacters */ false);
    }

    std::vector<Entry> results;

    QSqlQuery query{_manager->getDatabase()};
    query.prepare(
        //// Get list of entry ids whose jyutping starts with the queried string
        "WITH matching_entry_ids AS ( "
        "  SELECT rowid FROM entries WHERE jyutping GLOB ?"
        "), "
        " "
        //// Get the list of all definitions for those entries
        //// This CTE is used multiple times; would be nice if could materialize it
        "matching_definition_ids AS ( "
        "  SELECT definition_id, definition FROM definitions WHERE fk_entry_id "
        "  IN matching_entry_ids "
        "), "
        " "
        //// Get corresponding sentence ids for each of those definitions
        //// This CTE is used multiple times; would be nice if could materialize it
        "matching_chinese_sentence_ids AS ( "
        "  SELECT definition_id, fk_chinese_sentence_id "
        "  FROM matching_definition_ids AS mdi "
        "  JOIN definitions_chinese_sentences_links AS dcsl ON "
        "    mdi.definition_id = dcsl.fk_definition_id "
        "), "
        " "
        //// Get translations for each of the sentences
        "matching_translations AS ( "
        "  SELECT mcsi.fk_chinese_sentence_id, "
        "    json_group_array(DISTINCT "
        "      json_object('sentence', sentence, "
        "                  'language', language, "
        "                  'direct', direct "
        "    )) AS translation "
        "  FROM matching_chinese_sentence_ids AS mcsi "
        "  JOIN sentence_links AS sl ON mcsi.fk_chinese_sentence_id = "
        "    sl.fk_chinese_sentence_id "
        "  JOIN nonchinese_sentences AS ncs ON ncs.non_chinese_sentence_id = "
        "    sl.fk_non_chinese_sentence_id "
        "  GROUP BY mcsi.fk_chinese_sentence_id "
        "), "
        " "
        //// Get sentence data for each of the sentence ids
        "matching_sentences AS ( "
        " SELECT chinese_sentence_id, traditional, simplified, pinyin, "
        "   jyutping, language "
        " FROM chinese_sentences AS cs "
        " WHERE chinese_sentence_id IN ( "
        "   SELECT fk_chinese_sentence_id FROM "
        "     matching_chinese_sentence_ids "
        " ) "
        "),"
        " "
        //// Get translations for each of those sentences
        "matching_sentences_with_translations AS ( "
        "  SELECT chinese_sentence_id, "
        "    json_object('traditional', traditional, "
        "                'simplified', simplified, "
        "                'pinyin', pinyin, "
        "                'jyutping', jyutping, "
        "                'language', language, "
        "                'translations', json(translation)) AS sentence "
        "  FROM matching_sentences AS ms "
        "  LEFT JOIN matching_translations AS mt ON ms.chinese_sentence_id = "
        "    mt.fk_chinese_sentence_id "
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
        //// Create definition object with sentences for each definition
        "matching_definitions_with_sentences AS ( "
        "  SELECT fk_entry_id, fk_source_id, "
        "    json_object('definition', definition, "
        "                'label', label, 'sentences', "
        "                json_group_array(json(sentence))) AS definition "
        "  FROM matching_definitions AS md "
        "  LEFT JOIN matching_chinese_sentence_ids AS mcsi ON "
        "    md.definition_id = mcsi.definition_id "
        "  LEFT JOIN matching_sentences_with_translations AS mswt ON "
        "    mcsi.fk_chinese_sentence_id = mswt.chinese_sentence_id "
        "  GROUP BY md.definition_id "
        "), "
        " "
        //// Create definition groups for definitions of the same entry that come from the same source
        "matching_definition_groups AS ( "
        "  SELECT fk_entry_id, "
        "    json_object('source', sourcename, "
        "                'definitions', "
        "                json_group_array(json(definition))) AS definitions "
        "  FROM matching_definitions_with_sentences AS mdws "
        "  LEFT JOIN sources ON sources.source_id = mdws.fk_source_id "
        "  GROUP BY fk_entry_id, fk_source_id "
        "), "
        " "
        //// Construct the final entry object
        "matching_entries AS ( "
        "  SELECT simplified, traditional, jyutping, pinyin, "
        "    json_group_array(json(definitions)) AS definitions "
        "  FROM matching_definition_groups AS mdg "
        "  LEFT JOIN entries ON entries.entry_id = mdg.fk_entry_id "
        "  GROUP BY entry_id "
        "  ORDER BY frequency DESC "
        ") "
        " "
        "SELECT simplified, traditional, jyutping, pinyin, definitions FROM "
        "  matching_entries");

    // Don't add wildcard characters to GLOB term if searching for exact match
    const char *globJoinDelimiter = searchExactMatch ? "" : "?";
    std::string globTerm
        = ChineseUtils::constructRomanisationQuery(jyutpingWords,
                                                   globJoinDelimiter);

    query.addBindValue(
        QString{globTerm.c_str()}
        + QString{(searchExactMatch || dontAppendWildcard) ? "" : "*"});
    query.exec();

    // Do not parse results if new query has been made
    if (!checkQueryIDCurrent(queryID)) { return; }
    results = QueryParseUtils::parseEntries(query);

    if (!checkQueryIDCurrent(queryID)) { return; }
    notifyObserversIfQueryIdCurrent(results, /*emptyQuery=*/false, queryID);
}

void SQLSearch::searchPinyinThread(const QString &searchTerm,
                                   const unsigned long long queryID)
{
    // Replace "v" and "ü" with "u:" since "ü" is stored as "u:" in the table
    QString processedSearchTerm = searchTerm;
    int location = processedSearchTerm.indexOf("v");
    while (location != -1) {
        processedSearchTerm.remove(location, 1);
        processedSearchTerm.insert(location, "u:");
        location = processedSearchTerm.indexOf("v", location);
    }

    location = processedSearchTerm.indexOf("ü");
    while (location != -1) {
        processedSearchTerm.remove(location, 1);
        processedSearchTerm.insert(location, "u:");
        location = processedSearchTerm.indexOf("ü", location);
    }

    bool searchExactMatch = processedSearchTerm.at(0) == "\""
                            && processedSearchTerm.at(processedSearchTerm.size()
                                                      - 1)
                                   == "\""
                            && processedSearchTerm.length() >= 3;
    bool dontAppendWildcard = searchTerm.at(searchTerm.size() - 1) == "$";

    std::vector<std::string> pinyinWords;
    if (searchExactMatch) {
        QString searchTermWithoutQuotes
            = processedSearchTerm.mid(1, processedSearchTerm.size() - 2);
        Utils::split(searchTermWithoutQuotes.toStdString(), ' ', pinyinWords);
    } else {
        pinyinWords
            = ChineseUtils::segmentPinyin(processedSearchTerm,
                                          /* removeSpecialCharacters */ true,
                                          /* removeGlobCharacters */ false);
    }

    std::vector<Entry> results;

    QSqlQuery query{_manager->getDatabase()};
    query.prepare(
        //// Get list of entry ids whose pinyin starts with the queried string
        "WITH matching_entry_ids AS ( "
        "  SELECT rowid FROM entries WHERE pinyin GLOB ?"
        "), "
        " "
        //// Get the list of all definitions for those entries
        //// This CTE is used multiple times; would be nice if could materialize it
        "matching_definition_ids AS ( "
        "  SELECT definition_id, definition FROM definitions WHERE fk_entry_id "
        "    IN matching_entry_ids "
        "), "
        " "
        //// Get corresponding sentence ids for each of those definitions
        //// This CTE is used multiple times; would be nice if could materialize it
        "matching_chinese_sentence_ids AS ( "
        "  SELECT definition_id, fk_chinese_sentence_id "
        "  FROM matching_definition_ids AS mdi "
        "  JOIN definitions_chinese_sentences_links AS dcsl ON "
        "    mdi.definition_id = dcsl.fk_definition_id "
        "), "
        " "
        //// Get translations for each of the sentences
        "matching_translations AS ( "
        "  SELECT mcsi.fk_chinese_sentence_id, "
        "    json_group_array(DISTINCT "
        "      json_object('sentence', sentence, "
        "                  'language', language, "
        "                  'direct', direct "
        "    )) AS translation "
        "  FROM matching_chinese_sentence_ids AS mcsi "
        "  JOIN sentence_links AS sl ON mcsi.fk_chinese_sentence_id = "
        "    sl.fk_chinese_sentence_id "
        "  JOIN nonchinese_sentences AS ncs ON ncs.non_chinese_sentence_id = "
        "    sl.fk_non_chinese_sentence_id "
        "  GROUP BY mcsi.fk_chinese_sentence_id "
        "), "
        " "
        //// Get sentence data for each of the sentence ids
        "matching_sentences AS ( "
        " SELECT chinese_sentence_id, traditional, simplified, pinyin, "
        "   jyutping, language "
        " FROM chinese_sentences AS cs "
        " WHERE chinese_sentence_id IN ( "
        "   SELECT fk_chinese_sentence_id FROM matching_chinese_sentence_ids "
        " ) "
        "),"
        " "
        //// Get translations for each of those sentences
        "matching_sentences_with_translations AS ( "
        "  SELECT chinese_sentence_id, "
        "    json_object('traditional', traditional, "
        "                'simplified', simplified, "
        "                'pinyin', pinyin, "
        "                'jyutping', jyutping, "
        "                'language', language, "
        "                'translations', json(translation)) AS sentence "
        "  FROM matching_sentences AS ms "
        "  LEFT JOIN matching_translations AS mt ON ms.chinese_sentence_id = "
        "    mt.fk_chinese_sentence_id "
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
        //// Create definition object with sentences for each definition
        "matching_definitions_with_sentences AS ( "
        "  SELECT fk_entry_id, fk_source_id, "
        "    json_object('definition', definition, "
        "                'label', label, 'sentences', "
        "                json_group_array(json(sentence))) AS definition "
        "  FROM matching_definitions AS md "
        "  LEFT JOIN matching_chinese_sentence_ids AS mcsi ON "
        "    md.definition_id = mcsi.definition_id "
        "  LEFT JOIN matching_sentences_with_translations AS mswt ON "
        "    mcsi.fk_chinese_sentence_id = mswt.chinese_sentence_id "
        "  GROUP BY md.definition_id "
        "), "
        " "
        //// Create definition groups for definitions of the same entry that come from the same source
        "matching_definition_groups AS ( "
        "  SELECT fk_entry_id, "
        "    json_object('source', sourcename, "
        "                'definitions', "
        "                json_group_array(json(definition))) AS definitions "
        "  FROM matching_definitions_with_sentences AS mdws "
        "  LEFT JOIN sources ON sources.source_id = mdws.fk_source_id "
        "  GROUP BY fk_entry_id, fk_source_id "
        "), "
        " "
        //// Construct the final entry object
        "matching_entries AS ( "
        "  SELECT simplified, traditional, jyutping, pinyin, "
        "    json_group_array(json(definitions)) AS definitions "
        "  FROM matching_definition_groups AS mdg "
        "  LEFT JOIN entries ON entries.entry_id = mdg.fk_entry_id "
        "  GROUP BY entry_id "
        "  ORDER BY frequency DESC "
        ") "
        " "
        "SELECT simplified, traditional, jyutping, pinyin, definitions FROM "
        "  matching_entries");

    // Don't add wildcard characters to GLOB term if searching for exact match
    const char *globJoinDelimiter = searchExactMatch ? "" : "?";
    std::string globTerm
        = ChineseUtils::constructRomanisationQuery(pinyinWords,
                                                   globJoinDelimiter);

    query.addBindValue(
        QString{globTerm.c_str()}
        + QString{(searchExactMatch || dontAppendWildcard) ? "" : "*"});
    query.exec();

    // Do not parse results if new query has been made
    if (!checkQueryIDCurrent(queryID)) { return; }
    results = QueryParseUtils::parseEntries(query);

    if (!checkQueryIDCurrent(queryID)) { return; }
    notifyObserversIfQueryIdCurrent(results, /*emptyQuery=*/false, queryID);
}

void SQLSearch::searchEnglishThread(const QString &searchTerm,
                                    const unsigned long long queryID)
{
    bool searchExactMatch = searchTerm.at(0) == "\""
                            && searchTerm.at(searchTerm.size() - 1) == "\""
                            && searchTerm.length() >= 3;
    QString searchTermWithoutQuotes;
    if (searchExactMatch) {
        searchTermWithoutQuotes = searchTerm.mid(1, searchTerm.size() - 2);
    }

    std::vector<Entry> results;

    QSqlQuery query{_manager->getDatabase()};
    query.prepare("WITH "
                  "  matching_entry_ids "
                  "    AS ( "
                  "      SELECT "
                  "        fk_entry_id, "
                  "        rowid AS definition_id, "
                  "        bm25(definitions_fts, 0, 1) AS rank "
                  "      FROM "
                  "        definitions_fts "
                  "      WHERE "
                  "        definitions_fts MATCH ? "
                  "        AND definition LIKE ? "
                  "    ), "
                  "  matching_definition_ids "
                  "    AS ( "
                  "      SELECT "
                  "        definition_id, definition "
                  "      FROM "
                  "        definitions "
                  "      WHERE "
                  "        fk_entry_id "
                  "        IN (SELECT fk_entry_id FROM matching_entry_ids) "
                  "    ), "
                  "  definitions_and_ranks "
                  "    AS ( "
                  "      SELECT "
                  "        mdi.definition_id AS definition_id, "
                  "        mdi.definition AS definition, "
                  "        mei.rank AS rank "
                  "      FROM "
                  "        matching_definition_ids AS mdi "
                  "        LEFT JOIN matching_entry_ids AS mei ON "
                  "            mdi.definition_id = mei.definition_id "
                  "    ), "
                  "  matching_chinese_sentence_ids "
                  "    AS ( "
                  "      SELECT "
                  "        definition_id, fk_chinese_sentence_id "
                  "      FROM "
                  "        definitions_and_ranks AS mdi "
                  "        JOIN definitions_chinese_sentences_links AS dcsl ON "
                  "            mdi.definition_id = dcsl.fk_definition_id "
                  "    ), "
                  "  matching_translations "
                  "    AS ( "
                  "      SELECT "
                  "        mcsi.fk_chinese_sentence_id, "
                  "        json_group_array( "
                  "          DISTINCT "
                  "          json_object( "
                  "            'sentence', "
                  "            sentence, "
                  "            'language', "
                  "            language, "
                  "            'direct', "
                  "            direct "
                  "          ) "
                  "        ) "
                  "          AS translation "
                  "      FROM "
                  "        matching_chinese_sentence_ids AS mcsi "
                  "        JOIN sentence_links AS sl ON "
                  "            mcsi.fk_chinese_sentence_id "
                  "            = sl.fk_chinese_sentence_id "
                  "        JOIN nonchinese_sentences AS ncs ON "
                  "            ncs.non_chinese_sentence_id "
                  "            = sl.fk_non_chinese_sentence_id "
                  "      GROUP BY "
                  "        mcsi.fk_chinese_sentence_id "
                  "    ), "
                  "  matching_sentences "
                  "    AS ( "
                  "      SELECT "
                  "        chinese_sentence_id, "
                  "        traditional, "
                  "        simplified, "
                  "        pinyin, "
                  "        jyutping, "
                  "        language "
                  "      FROM "
                  "        chinese_sentences AS cs "
                  "      WHERE "
                  "        chinese_sentence_id "
                  "        IN ( "
                  "            SELECT "
                  "              fk_chinese_sentence_id "
                  "            FROM "
                  "              matching_chinese_sentence_ids "
                  "          ) "
                  "    ), "
                  "  matching_sentences_with_translations "
                  "    AS ( "
                  "      SELECT "
                  "        chinese_sentence_id, "
                  "        json_object( "
                  "          'traditional', "
                  "          traditional, "
                  "          'simplified', "
                  "          simplified, "
                  "          'pinyin', "
                  "          pinyin, "
                  "          'jyutping', "
                  "          jyutping, "
                  "          'language', "
                  "          language, "
                  "          'translations', "
                  "          json(translation) "
                  "        ) "
                  "          AS sentence "
                  "      FROM "
                  "        matching_sentences AS ms "
                  "        LEFT JOIN matching_translations AS mt ON "
                  "            ms.chinese_sentence_id "
                  "            = mt.fk_chinese_sentence_id "
                  "    ), "
                  "  matching_definitions "
                  "    AS ( "
                  "      SELECT "
                  "        mdi.definition_id, "
                  "        d.fk_entry_id, "
                  "        d.fk_source_id, "
                  "        d.definition, "
                  "        d.label, "
                  "        rank "
                  "      FROM "
                  "        definitions_and_ranks AS mdi "
                  "        JOIN definitions AS d ON "
                  "            mdi.definition_id = d.definition_id "
                  "    ), "
                  "  matching_definitions_with_sentences "
                  "    AS ( "
                  "      SELECT "
                  "        fk_entry_id, "
                  "        fk_source_id, "
                  "        md.rank AS rank, "
                  "        json_object( "
                  "          'definition', "
                  "          definition, "
                  "          'label', "
                  "          label, "
                  "          'sentences', "
                  "          json_group_array(json(sentence)) "
                  "        ) "
                  "          AS definition "
                  "      FROM "
                  "        matching_definitions AS md "
                  "        LEFT JOIN matching_chinese_sentence_ids AS mcsi ON "
                  "            md.definition_id = mcsi.definition_id "
                  "        LEFT JOIN matching_sentences_with_translations "
                  "            AS mswt ON "
                  "            mcsi.fk_chinese_sentence_id "
                  "            = mswt.chinese_sentence_id "
                  "      GROUP BY "
                  "        md.definition_id "
                  "    ), "
                  "  matching_definition_groups "
                  "    AS ( "
                  "      SELECT "
                  "        fk_entry_id, "
                  "        CASE sourceshortname "
                  "        WHEN 'ABY' THEN avg(mdws.rank) * 3 "
                  "        WHEN 'CCY' THEN avg(mdws.rank) * 3 "
                  "        WHEN 'WHK' THEN avg(mdws.rank) * 3 "
                  "        ELSE avg(mdws.rank) "
                  "        END "
                  "          AS rank, "
                  "        json_object( "
                  "          'source', "
                  "          sourcename, "
                  "          'definitions', "
                  "          json_group_array(json(definition)) "
                  "        ) "
                  "          AS definitions "
                  "      FROM "
                  "        matching_definitions_with_sentences AS mdws "
                  "        LEFT JOIN sources ON "
                  "            sources.source_id = mdws.fk_source_id "
                  "      GROUP BY "
                  "        fk_entry_id, fk_source_id "
                  "    ), "
                  "  matching_entries "
                  "    AS ( "
                  "      SELECT "
                  "        simplified, "
                  "        traditional, "
                  "        jyutping, "
                  "        pinyin, "
                  "        sum(rank) AS rank, "
                  "        json_group_array(json(definitions)) AS definitions "
                  "      FROM "
                  "        matching_definition_groups AS mdg "
                  "        LEFT JOIN entries ON "
                  "            entries.entry_id = mdg.fk_entry_id "
                  "      GROUP BY "
                  "        entry_id "
                  "      ORDER BY "
                  "        rank ASC, frequency DESC "
                  "    ) "
                  "SELECT "
                  "  rank, "
                  "  simplified, "
                  "  traditional, "
                  "  jyutping, "
                  "  pinyin, "
                  "  definitions "
                  "FROM "
                  "  matching_entries; ");
    if (searchExactMatch) {
        query.addBindValue("\"" + searchTermWithoutQuotes + "\"");
        query.addBindValue(searchTermWithoutQuotes);
    } else {
        query.addBindValue("\"" + searchTerm + "\"");
        query.addBindValue("%" + searchTerm + "%");
    }
    query.setForwardOnly(true);
    query.exec();

    // Do not parse results if new query has been made
    if (!checkQueryIDCurrent(queryID)) {
        return;
    }
    results = QueryParseUtils::parseEntries(query);

    if (!checkQueryIDCurrent(queryID)) {
        return;
    }
    notifyObserversIfQueryIdCurrent(results, /*emptyQuery=*/false, queryID);
}

// To seach by unique, select by all the attributes that we have.
void SQLSearch::searchByUniqueThread(const QString &simplified,
                                     const QString &traditional,
                                     const QString &jyutping,
                                     const QString &pinyin,
                                     const unsigned long long queryID)
{
    std::vector<Entry> results;

    QSqlQuery query{_manager->getDatabase()};
    query.prepare(
        //// Get list of entry ids whose traditional form matches the query
        "WITH matching_entry_ids AS ( "
        "  SELECT rowid FROM entries WHERE "
        "    simplified LIKE ? "
        "    AND traditional LIKE ? "
        "    AND jyutping LIKE ? "
        "    AND pinyin LIKE ? "
        "), "
        " "
        //// Get the list of all definitions for those entries
        //// This CTE is used multiple times; would be nice if could materialize it
        "matching_definition_ids AS ( "
        "  SELECT definition_id, definition FROM definitions WHERE fk_entry_id "
        "    IN matching_entry_ids "
        "), "
        " "
        //// Get corresponding sentence ids for each of those definitions
        //// This CTE is used multiple times; would be nice if could materialize it
        "matching_chinese_sentence_ids AS ( "
        "  SELECT definition_id, fk_chinese_sentence_id "
        "  FROM matching_definition_ids AS mdi "
        "  JOIN definitions_chinese_sentences_links AS dcsl ON "
        "    mdi.definition_id = dcsl.fk_definition_id "
        "), "
        " "
        //// Get translations for each of the sentences
        "matching_translations AS ( "
        "  SELECT mcsi.fk_chinese_sentence_id, "
        "    json_group_array(DISTINCT "
        "      json_object('sentence', sentence, "
        "                  'language', language, "
        "                  'direct', direct "
        "    )) AS translation "
        "  FROM matching_chinese_sentence_ids AS mcsi "
        "  JOIN sentence_links AS sl ON mcsi.fk_chinese_sentence_id = "
        "    sl.fk_chinese_sentence_id "
        "  JOIN nonchinese_sentences AS ncs ON ncs.non_chinese_sentence_id = "
        "    sl.fk_non_chinese_sentence_id "
        "  GROUP BY mcsi.fk_chinese_sentence_id "
        "), "
        " "
        //// Get sentence data for each of the sentence ids
        "matching_sentences AS ( "
        "  SELECT chinese_sentence_id, traditional, simplified, pinyin, "
        "    jyutping, language "
        "  FROM chinese_sentences AS cs "
        "  WHERE chinese_sentence_id IN ( "
        "    SELECT fk_chinese_sentence_id FROM matching_chinese_sentence_ids "
        "  ) "
        "),"
        " "
        //// Get translations for each of those sentences
        "matching_sentences_with_translations AS ( "
        "  SELECT chinese_sentence_id, "
        "    json_object('traditional', traditional, "
        "                'simplified', simplified, "
        "                'pinyin', pinyin, "
        "                'jyutping', jyutping, "
        "                'language', language, "
        "                'translations', json(translation)) AS sentence "
        "  FROM matching_sentences AS ms "
        "  LEFT JOIN matching_translations AS mt ON ms.chinese_sentence_id = "
        "    mt.fk_chinese_sentence_id "
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
        //// Create definition object with sentences for each definition
        "matching_definitions_with_sentences AS ( "
        " SELECT md.fk_entry_id, md.fk_source_id, "
        "   json_object('definition', md.definition, "
        "               'label', label, 'sentences', "
        "               json_group_array(json(sentence))) AS definition "
        " FROM matching_definitions AS md "
        " LEFT JOIN matching_chinese_sentence_ids AS mcsi ON "
        "   md.definition_id = mcsi.definition_id "
        " LEFT JOIN matching_sentences_with_translations AS mswt ON "
        "   mcsi.fk_chinese_sentence_id = mswt.chinese_sentence_id "
        " GROUP BY md.definition_id "
        "), "
        " "
        //// Create definition groups for definitions of the same entry that come from the same source
        "matching_definition_groups AS ( "
        "  SELECT fk_entry_id, "
        "    json_object('source', sourcename, "
        "                'definitions', "
        "                json_group_array(json(definition))) AS definitions "
        "  FROM matching_definitions_with_sentences AS mdws "
        "  LEFT JOIN sources ON sources.source_id = mdws.fk_source_id "
        "  GROUP BY fk_entry_id, fk_source_id "
        "), "
        " "
        //// Construct the final entry object
        "matching_entries AS ( "
        "  SELECT simplified, traditional, jyutping, pinyin, "
        "    json_group_array(json(definitions)) AS definitions "
        "  FROM matching_definition_groups AS mdg "
        "  LEFT JOIN entries ON entries.entry_id = mdg.fk_entry_id "
        "  GROUP BY entry_id "
        "  ORDER BY frequency DESC "
        ") "
        " "
        "SELECT simplified, traditional, jyutping, pinyin, definitions FROM "
        "  matching_entries");
    query.addBindValue(simplified);
    query.addBindValue(traditional);
    query.addBindValue(jyutping);
    query.addBindValue(pinyin);
    query.setForwardOnly(true);
    query.exec();

    // Do not parse results if new query has been made
    if (!checkQueryIDCurrent(queryID)) {
        return;
    }
    results = QueryParseUtils::parseEntries(query);

    if (!checkQueryIDCurrent(queryID)) {
        return;
    }
    notifyObserversIfQueryIdCurrent(results, /*emptyQuery=*/false, queryID);
}

// To search for sentences, use the sentence_links table to JOIN
// between the chinese and non_chinese_sentences tables.
void SQLSearch::searchTraditionalSentencesThread(const QString &searchTerm,
                                                 const unsigned long long queryID)
{
    std::vector<SourceSentence> results;

    QSqlQuery query{_manager->getDatabase()};
    query.prepare(
        "WITH matching_chinese_sentence_ids AS ( "
        "  SELECT chinese_sentence_id "
        "  FROM chinese_sentences "
        "  WHERE traditional LIKE ? ESCAPE '\\'"
        "), "
        " "
        //// Get translations for each of the sentence
        "translations_with_source AS ( "
        "  SELECT s.sourcename AS source, "
        "    mcsi.chinese_sentence_id AS chinese_sentence_id, "
        "      json_group_array(distinct "
        "    json_object('sentence', sentence, "
        "                'language', language, "
        "                'direct', direct)) AS translation "
        "  FROM matching_chinese_sentence_ids AS mcsi "
        "  LEFT JOIN sentence_links AS sl ON mcsi.chinese_sentence_id = "
        "    sl.fk_chinese_sentence_id "
        "  LEFT JOIN nonchinese_sentences AS ncs ON "
        "    ncs.non_chinese_sentence_id = sl.fk_non_chinese_sentence_id "
        "  LEFT JOIN sources AS s ON s.source_id = sl.fk_source_id"
        "  GROUP BY s.sourcename, mcsi.chinese_sentence_id"
        "), "
        ""
        //// Group translations by source
        "matching_translations AS ( "
        "  SELECT chinese_sentence_id, "
        "     json_group_array( "
        "       json_object('source', source, "
        "                   'translations', json(translation)) "
        "     ) AS translations "
        "  FROM translations_with_source AS tws "
        "  GROUP BY chinese_sentence_id "
        "), "
        " "
        //// Get sentence data for each of the sentence id
        "matching_sentences AS ( "
        " SELECT chinese_sentence_id, traditional, simplified, pinyin, "
        "   jyutping, language "
        " FROM chinese_sentences AS cs "
        " WHERE chinese_sentence_id IN ( "
        "   SELECT chinese_sentence_id FROM matching_chinese_sentence_ids "
        " ) "
        "), "
        " "
        //// Match up the translations with their sentences, and get the linked
        //// definition's source name if it exists
        "matching_sentences_with_translations AS ( "
        "  SELECT max(sourcename) AS sourcename, traditional, simplified, "
        "    pinyin, jyutping, language, translations "
        "  FROM matching_sentences AS ms "
        "  LEFT JOIN matching_translations AS mt ON ms.chinese_sentence_id = "
        "    mt.chinese_sentence_id "
        "  LEFT JOIN definitions_chinese_sentences_links AS dcsl ON "
        "    ms.chinese_sentence_id = dcsl.fk_chinese_sentence_id "
        "  LEFT JOIN definitions AS d ON dcsl.fk_definition_id = "
        "    d.definition_id "
        "  LEFT JOIN sources AS s ON d.fk_source_id = s.source_id "
        "  GROUP BY traditional, simplified, pinyin, jyutping, language, "
        "    translations "
        "  ORDER BY ms.chinese_sentence_id "
        ") "
        " "
        "SELECT sourcename, traditional, simplified, pinyin, jyutping, "
        "  language, translations FROM matching_sentences_with_translations ");
    query.addBindValue("%" + searchTerm + "%");
    query.exec();

    if (!checkQueryIDCurrent(queryID)) {
        return;
    }
    results = QueryParseUtils::parseSentences(query);

    if (!checkQueryIDCurrent(queryID)) { return; }
    notifyObserversIfQueryIdCurrent(results, /*emptyQuery=*/false, queryID);
}

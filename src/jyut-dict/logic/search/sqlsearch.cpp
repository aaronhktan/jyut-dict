#include "sqlsearch.h"

#include "logic/database/queryparseutils.h"
#include "logic/search/searchqueries.h"
#include "logic/settings/settingsutils.h"
#include "logic/utils/cantoneseutils.h"
#include "logic/utils/chineseutils.h"
#include "logic/utils/mandarinutils.h"
#include "logic/utils/scriptdetector.h"
#include "logic/utils/utils.h"

#include <QString>
#include <QtConcurrent/QtConcurrent>

#ifdef Q_OS_WIN
#include <cctype>
#endif

namespace {

void prepareJyutpingBindValues(const QString &searchTerm,
                               QString &regexTerm,
                               bool fuzzyJyutping,
                               bool unsafeFuzzyJyutping)
{
    // When the search term is surrounded by quotes, search for only term
    // inside quotes (not the quotes themselves)
    // Unlike the simplified/traditional search, only trigger exact match
    // searching if enclosed by Western quotes (U+0022).
    bool searchExactMatch = searchTerm.startsWith("\"")
                            && searchTerm.endsWith("\"")
                            && searchTerm.length() >= 3;
    bool dontAppendWildcard = searchTerm.endsWith("$");

    // Attempt to correct romanization issues (but not pronunciation!)
    QString correctedSearchTerm{searchTerm};

    if (!searchExactMatch && fuzzyJyutping) {
        CantoneseUtils::jyutpingAutocorrect(
            searchTerm.chopped(dontAppendWildcard ? 1 : 0),
            correctedSearchTerm,
            /* unsafeSubstitutions */ unsafeFuzzyJyutping);
    }

    std::vector<std::string> jyutpingSyllables;
    if (searchExactMatch) {
        Utils::split(QStringView{correctedSearchTerm.constBegin() + 1,
                                 correctedSearchTerm.constEnd() - 1}
                         .toString()
                         .toStdString(),
                     ' ',
                     jyutpingSyllables);
    } else {
        CantoneseUtils::segmentJyutping(
            correctedSearchTerm,
            jyutpingSyllables,
            /* removeSpecialCharacters */ true,
            /* removeGlobCharacters */ false,
            /* removeRegexCharacters= */ !fuzzyJyutping);
    }

    if (!searchExactMatch && fuzzyJyutping) {
        // Attempt to broaden search for sound changes (e.g. nei5 -> lei5)
        CantoneseUtils::jyutpingSoundChanges(jyutpingSyllables);
    }

    // Don't add wildcard characters to GLOB term if searching for exact match
    const char *globJoinDelimiter = searchExactMatch ? "" : "?";
    std::string query
        = ChineseUtils::constructRomanisationQuery(jyutpingSyllables,
                                                   globJoinDelimiter);

    if (fuzzyJyutping) {
        regexTerm = QString{"^"}
                    + QString::fromStdString(query)
                          .replace("*", ".*") // Convert glob characters to regex
                          .replace("?", ".")
                          .replace("!", "?") // Workaround for glob  replacement
                    + QString{(searchExactMatch || dontAppendWildcard) ? "$"
                                                                       : ".*$"};
    } else {
        regexTerm = QString::fromStdString(query)
                    + QString{(searchExactMatch || dontAppendWildcard) ? ""
                                                                       : "*"};
    }
}

void preparePinyinBindValues(const QString &searchTerm,
                             QString &regexTerm,
                             bool fuzzyPinyin)
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

    std::vector<std::string> pinyinSyllables;
    if (searchExactMatch) {
        QString searchTermWithoutQuotes
            = processedSearchTerm.mid(1, processedSearchTerm.size() - 2);
        Utils::split(searchTermWithoutQuotes.toStdString(),
                     ' ',
                     pinyinSyllables);
    } else {
        MandarinUtils::segmentPinyin(processedSearchTerm,
                                     pinyinSyllables,
                                     /* removeSpecialCharacters */ true,
                                     /* removeGlobCharacters */ false);
    }

    if (!searchExactMatch && fuzzyPinyin) {
        MandarinUtils::pinyinSoundChanges(pinyinSyllables);
    }

    // Don't add wildcard characters to GLOB term if searching for exact match
    const char *globJoinDelimiter = searchExactMatch ? "" : "?";
    std::string query
        = ChineseUtils::constructRomanisationQuery(pinyinSyllables,
                                                   globJoinDelimiter);

    if (fuzzyPinyin) {
        regexTerm = QString{"^"}
                    + QString::fromStdString(query)
                          .replace("*", ".*") // Convert glob characters to regex
                          .replace("?", ".")
                          .replace("!", "?") // Workaround for glob  replacement
                    + QString{(searchExactMatch || dontAppendWildcard) ? "$"
                                                                       : ".*$"};
    } else {
        regexTerm = QString::fromStdString(query)
                    + QString{(searchExactMatch || dontAppendWildcard) ? ""
                                                                       : "*"};
    }
}

} // namespace

SQLSearch::SQLSearch()
{
    std::random_device rd;
    _generator = std::mt19937_64{rd()};
    _settings = Settings::getSettings();
}

SQLSearch::SQLSearch(std::shared_ptr<SQLDatabaseManager> manager)
    : _manager{manager}
{
    std::random_device rd;
    _generator = std::mt19937_64{rd()};
    _settings = Settings::getSettings();
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
void SQLSearch::notifyObservers(SearchParameters params)
{
    std::list<ISearchObserver *>::const_iterator it = _observers.begin();
    while (it != _observers.end()) {
        (static_cast<ISearchObserver *>(*it))->detectedLanguage(params);
        ++it;
    }
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

void SQLSearch::notifyObserversIfQueryIdCurrent(SearchParameters params,
                                                const unsigned long long queryID)
{
    std::lock_guard<std::mutex> notifyLock{_notifyMutex};
    if (queryID != _queryID) {
        return;
    }

    notifyObservers(params);
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
    runThread(&SQLSearch::searchSimplifiedThread,
              searchTerm.normalized(QString::NormalizationForm_C),
              queryID);
}

void SQLSearch::searchTraditional(const QString &searchTerm)
{
    unsigned long long queryID = generateAndSetQueryID();
    runThread(&SQLSearch::searchTraditionalThread,
              searchTerm.normalized(QString::NormalizationForm_C),
              queryID);
}

void SQLSearch::searchJyutping(const QString &searchTerm)
{
    unsigned long long queryID = generateAndSetQueryID();
    runThread(&SQLSearch::searchJyutpingThread,
              searchTerm.normalized(QString::NormalizationForm_C),
              queryID);
}

void SQLSearch::searchPinyin(const QString &searchTerm)
{
    unsigned long long queryID = generateAndSetQueryID();
    runThread(&SQLSearch::searchPinyinThread,
              searchTerm.normalized(QString::NormalizationForm_C),
              queryID);
}

void SQLSearch::searchEnglish(const QString &searchTerm)
{
    unsigned long long queryID = generateAndSetQueryID();
    runThread(&SQLSearch::searchEnglishThread,
              searchTerm.normalized(QString::NormalizationForm_C),
              queryID);
}

void SQLSearch::searchAutoDetect(const QString &searchTerm)
{
    unsigned long long queryId = generateAndSetQueryID();
    runThread(&SQLSearch::searchAutoDetectThread,
              searchTerm.normalized(QString::NormalizationForm_C),
              queryId);
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
    auto future
        = QtConcurrent::run(&SQLSearch::searchByUniqueThread,
                            this,
                            simplified.normalized(QString::NormalizationForm_C),
                            traditional.normalized(QString::NormalizationForm_C),
                            jyutping.normalized(QString::NormalizationForm_C),
                            pinyin.normalized(QString::NormalizationForm_C),
                            queryID);
    _futures.append(future);
}

void SQLSearch::searchTraditionalSentences(const QString &searchTerm)
{
    unsigned long long queryID = generateAndSetQueryID();
    runThread(&SQLSearch::searchTraditionalSentencesThread,
              searchTerm.normalized(QString::NormalizationForm_C),
              queryID);
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

    auto future = QtConcurrent::run(threadFunction, this, searchTerm, queryID);
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
    bool searchExactMatch
        = ((searchTerm.startsWith("\"") && searchTerm.endsWith("\""))
           || (searchTerm.startsWith("”") && searchTerm.endsWith("“")))
          && searchTerm.length() >= 3;
    bool dontAppendWildcard = searchTerm.endsWith("$");

    QString searchTermWithoutQuotes;
    QString searchTermWithoutEndPositionMarker;
    if (searchExactMatch) {
        searchTermWithoutQuotes = searchTerm.mid(1, searchTerm.size() - 2);
    } else if (dontAppendWildcard) {
        searchTermWithoutEndPositionMarker = searchTerm.chopped(1);
    }

    std::vector<Entry> results;

    QSqlQuery query{_manager->getDatabase()};
    query.prepare(SEARCH_SIMPLIFIED_QUERY);
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
    bool searchExactMatch
        = ((searchTerm.startsWith("\"") && searchTerm.endsWith("\""))
           || (searchTerm.startsWith("“") && searchTerm.endsWith("”")))
          && searchTerm.length() >= 3;
    bool dontAppendWildcard = searchTerm.endsWith("$");

    QString searchTermWithoutQuotes;
    QString searchTermWithoutEndPositionMarker;
    if (searchExactMatch) {
        searchTermWithoutQuotes = searchTerm.mid(1, searchTerm.size() - 2);
    } else if (dontAppendWildcard) {
        searchTermWithoutEndPositionMarker = searchTerm.chopped(1);
    }

    std::vector<Entry> results;

    QSqlQuery query{_manager->getDatabase()};
    query.prepare(SEARCH_TRADITIONAL_QUERY);
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
    std::vector<Entry> results;

    QSqlQuery query{_manager->getDatabase()};

    bool fuzzyJyutping
        = _settings->value("Search/fuzzyJyutping", QVariant{true}).toBool();
    bool unsafeFuzzyJyutping = _settings
                                   ->value("Search/dangerousFuzzyJyutping",
                                           QVariant{false})
                                   .toBool();

    query.prepare(QString{SEARCH_JYUTPING_QUERY}.arg(fuzzyJyutping ? REGEXP_STR
                                                                   : GLOB_STR));

    QString globTerm;
    prepareJyutpingBindValues(searchTerm,
                              globTerm,
                              fuzzyJyutping,
                              unsafeFuzzyJyutping);
    query.addBindValue(globTerm);
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
    std::vector<Entry> results;

    QSqlQuery query{_manager->getDatabase()};

    bool fuzzyPinyin
        = _settings->value("Search/fuzzyPinyin", QVariant{true}).toBool();

    query.prepare(
        QString{SEARCH_PINYIN_QUERY}.arg(fuzzyPinyin ? REGEXP_STR : GLOB_STR));

    QString globTerm;
    preparePinyinBindValues(searchTerm, globTerm, fuzzyPinyin);
    query.addBindValue(globTerm);
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
    bool searchExactMatch = searchTerm.startsWith("\"")
                            && searchTerm.endsWith("\"")
                            && searchTerm.length() >= 3;
    QString searchTermWithoutQuotes;
    if (searchExactMatch) {
        searchTermWithoutQuotes = searchTerm.mid(1, searchTerm.size() - 2);
    }

    std::vector<Entry> results;

    QSqlQuery query{_manager->getDatabase()};
    query.prepare(SEARCH_ENGLISH_QUERY);
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

void SQLSearch::searchAutoDetectThread(const QString &searchTerm,
                                       const unsigned long long queryID)
{
    ScriptDetector sd{searchTerm};
    if (sd.containsSimplifiedChinese()) {
        notifyObserversIfQueryIdCurrent(SearchParameters::SIMPLIFIED, queryID);
        searchSimplifiedThread(searchTerm, queryID);
        return;
    }
    if (sd.containsTraditionalChinese() || sd.containsChinese()) {
        notifyObserversIfQueryIdCurrent(SearchParameters::TRADITIONAL, queryID);
        searchTraditionalThread(searchTerm, queryID);
        return;
    }

    bool fuzzyJyutping
        = _settings->value("Search/fuzzyJyutping", QVariant{true}).toBool();
    bool unsafeFuzzyJyutping = _settings
                                   ->value("Search/unsafeFuzzyJyutping",
                                           QVariant{false})
                                   .toBool();
    bool fuzzyPinyin
        = _settings->value("Search/fuzzyPinyin", QVariant{true}).toBool();

    QSqlQuery jyutpingQuery{_manager->getDatabase()};
    jyutpingQuery.prepare(QString{SEARCH_JYUTPING_EXISTS_QUERY}.arg(
        fuzzyJyutping ? REGEXP_STR : GLOB_STR));
    QString jyutpingSearchTerm;
    prepareJyutpingBindValues(searchTerm,
                              jyutpingSearchTerm,
                              fuzzyJyutping,
                              unsafeFuzzyJyutping);
    jyutpingQuery.addBindValue(jyutpingSearchTerm);
    jyutpingQuery.setForwardOnly(true);
    jyutpingQuery.exec();
    bool jyutpingExists = QueryParseUtils::parseExistence(jyutpingQuery);

    if (jyutpingExists) {
        notifyObserversIfQueryIdCurrent(SearchParameters::JYUTPING, queryID);
        searchJyutpingThread(searchTerm, queryID);
        return;
    }

    QSqlQuery pinyinQuery{_manager->getDatabase()};
    pinyinQuery.prepare(QString{SEARCH_PINYIN_EXISTS_QUERY}.arg(
        fuzzyPinyin ? REGEXP_STR : GLOB_STR));
    QString pinyinSearchTerm;
    preparePinyinBindValues(searchTerm, pinyinSearchTerm, fuzzyPinyin);
    pinyinQuery.addBindValue(pinyinSearchTerm);
    pinyinQuery.setForwardOnly(true);
    pinyinQuery.exec();
    bool pinyinExists = QueryParseUtils::parseExistence(pinyinQuery);

    if (pinyinExists) {
        notifyObserversIfQueryIdCurrent(SearchParameters::PINYIN, queryID);
        searchPinyinThread(searchTerm, queryID);
        return;
    }

    notifyObserversIfQueryIdCurrent(SearchParameters::ENGLISH, queryID);
    searchEnglishThread(searchTerm, queryID);
    return;
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
    query.prepare(SEARCH_UNIQUE_QUERY);
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
    query.prepare(SEARCH_TRADITIONAL_SENTENCES_QUERY);
    query.addBindValue("%" + searchTerm + "%");
    query.exec();

    if (!checkQueryIDCurrent(queryID)) {
        return;
    }
    results = QueryParseUtils::parseSentences(query);

    if (!checkQueryIDCurrent(queryID)) { return; }
    notifyObserversIfQueryIdCurrent(results, /*emptyQuery=*/false, queryID);
}

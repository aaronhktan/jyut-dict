#include "sqlsearch.h"

#include "logic/database/queryparseutils.h"
#include "logic/database/sqldatabasemanager.h"
#include "logic/database/sqldatabaseutils.h"
#include "logic/entry/entry.h"
#include "logic/search/searchqueries.h"
#include "logic/settings/settingsutils.h"
#include "logic/source/sourcemetadata.h"
#include "logic/utils/cantoneseutils.h"
#include "logic/utils/chineseutils.h"
#include "logic/utils/mandarinutils.h"
#include "logic/utils/scriptdetector.h"
#include "logic/utils/utils.h"
#include "logic/utils/utils_qt.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QString>
#include <QtConcurrent/QtConcurrent>

#include <numeric>

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
                    % QString::fromStdString(query)
                          .replace("*", ".*") // Convert glob characters to regex
                          .replace("?", ".")
                          .replace("!", "?") // Workaround for glob  replacement
                    % QString{(searchExactMatch || dontAppendWildcard) ? "$"
                                                                       : ".*$"};
    } else {
        regexTerm = QString::fromStdString(query)
                    % QString{(searchExactMatch || dontAppendWildcard) ? ""
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
                    % QString::fromStdString(query)
                          .replace("*", ".*") // Convert glob characters to regex
                          .replace("?", ".")
                          .replace("!", "?") // Workaround for glob  replacement
                    % QString{(searchExactMatch || dontAppendWildcard) ? "$"
                                                                       : ".*$"};
    } else {
        regexTerm = QString::fromStdString(query)
                    % QString{(searchExactMatch || dontAppendWildcard) ? ""
                                                                       : "*"};
    }
}

std::vector<QString> literalPrefixes(const QString &regex)
{
    // Truncate string to right before a second "?" or "(", whichever occurs
    // earlier. (The limitation of unfoldBasicRegex is that we only support
    // one set of parentheses and one question mark.)
    qsizetype endIdx = regex.size();
    qsizetype firstIdx = regex.indexOf("(");
    if (firstIdx != -1) {
        qsizetype secondIdx = regex.indexOf("(", firstIdx + 1);
        if (secondIdx != -1) {
            endIdx = secondIdx;
        }
    }
    firstIdx = regex.indexOf("?");
    if (firstIdx != -1) {
        qsizetype secondIdx = regex.indexOf("?", firstIdx + 1);
        if (secondIdx != -1) {
            endIdx = std::min(endIdx, secondIdx);
        }
    }

    std::vector<QString> possibilities;
    Utils::unfoldBasicRegex(regex.first(endIdx), possibilities);

    std::unordered_set<QString> prefixes;
    for (const auto &p : possibilities) {
        QString prefix;

        QString::const_iterator it;
        for (it = p.cbegin(); it != p.cend(); it++) {
            if (!(*it).isLetterOrNumber()) {
                break;
            }
            prefix += *it;
        }

        if (it == p.cend()) {
            if (!prefix.isEmpty()) {
                prefixes.insert(prefix);
            }
            continue;
        }

        if (*it == '*' || *it == '?') {
            if (!prefix.isEmpty()) {
                prefix = prefix.chopped(1);
            }
            continue;
        }

        if (!prefix.isEmpty()) {
            prefixes.insert(prefix);
        }
    }

    std::vector<QString> ret{prefixes.begin(), prefixes.end()};
    return ret;
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
    std::lock_guard lock(_watchers.mut);
    for (const auto watcher : _watchers.set) {
        watcher->future().waitForFinished();
        delete watcher;
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
void SQLSearch::notifyObservers(const std::vector<Example> &results,
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

void SQLSearch::notifyObserversIfQueryIdCurrent(
    const std::vector<Example> &results,
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

void SQLSearch::populateJyutpingCache(unsigned int hash)
{
    QSqlQuery query{_manager->getDatabase()};
    query.prepare(FETCH_ALL_JYUTPING_QUERY);
    query.setForwardOnly(true);
    query.exec();

    auto newCache = std::make_shared<std::vector<QString>>();
    const int jyutpingIndex = query.record().indexOf("jyutping");
    while (query.next()) {
        newCache->emplace_back(query.value(jyutpingIndex).toString());
    }

    _jyutpingCache = std::make_tuple(hash, newCache);
}

void SQLSearch::populatePinyinCache(unsigned int hash)
{
    QSqlQuery query{_manager->getDatabase()};
    query.prepare(FETCH_ALL_PINYIN_QUERY);
    query.setForwardOnly(true);
    query.exec();

    auto newCache = std::make_shared<std::vector<QString>>();
    const int pinyinIndex = query.record().indexOf("pinyin");
    while (query.next()) {
        newCache->emplace_back(query.value(pinyinIndex).toString());
    }

    _pinyinCache = std::make_tuple(hash, newCache);
}

std::vector<QString> SQLSearch::jyutpingFilteredBy(const QString &prefix)
{
    QSqlQuery query{_manager->getDatabase()};
    query.prepare(FETCH_JYUTPING_WITH_PREFIX_QUERY);
    query.addBindValue(prefix + "*");
    query.setForwardOnly(true);
    query.exec();

    std::vector<QString> jyutping;
    const int jyutpingIndex = query.record().indexOf("jyutping");
    while (query.next()) {
        jyutping.emplace_back(query.value(jyutpingIndex).toString());
    }

    return jyutping;
}

std::vector<QString> SQLSearch::pinyinFilteredBy(const QString &prefix)
{
    QSqlQuery query{_manager->getDatabase()};
    query.prepare(FETCH_PINYIN_WITH_PREFIX_QUERY);
    query.addBindValue(prefix + "*");
    query.setForwardOnly(true);
    query.exec();

    std::vector<QString> pinyin;
    const int pinyinIndex = query.record().indexOf("pinyin");
    while (query.next()) {
        pinyin.emplace_back(query.value(pinyinIndex).toString());
    }

    return pinyin;
}

std::shared_ptr<std::vector<QString>> SQLSearch::allJyutping()
{
    std::vector<SourceMetadata> sources;
    SQLDatabaseUtils utils;
    QSqlDatabase db{_manager->getDatabase()};
    utils.readSources(db, sources);

    unsigned int hash = std::reduce(sources.cbegin(),
                                    sources.cend(),
                                    0,
                                    [](unsigned int a, const SourceMetadata &b) {
                                        return qHashMulti(0, a, b);
                                    });

    {
        std::shared_lock lock(_jyutpingCacheMutex);
        if (_jyutpingCache.has_value()) {
            if (hash == std::get<0>(_jyutpingCache.value())) {
                return std::get<1>(_jyutpingCache.value());
            }
        }
    }

    std::lock_guard lock{_jyutpingCacheMutex};
    if (!_jyutpingCache.has_value()
        || hash != std::get<0>(_jyutpingCache.value())) {
        populateJyutpingCache(hash);
    }

    return std::get<1>(_jyutpingCache.value());
}

std::shared_ptr<std::vector<QString>> SQLSearch::allPinyin()
{
    std::vector<SourceMetadata> sources;
    SQLDatabaseUtils utils;
    QSqlDatabase db{_manager->getDatabase()};
    utils.readSources(db, sources);

    unsigned int hash = std::reduce(sources.cbegin(),
                                    sources.cend(),
                                    0,
                                    [](unsigned int a, const SourceMetadata &b) {
                                        return qHashMulti(0, a, b);
                                    });

    {
        std::shared_lock lock(_pinyinCacheMutex);
        if (_pinyinCache.has_value()) {
            if (hash == std::get<0>(_pinyinCache.value())) {
                return std::get<1>(_pinyinCache.value());
            }
        }
    }

    std::lock_guard lock{_pinyinCacheMutex};
    if (!_pinyinCache.has_value() || hash != std::get<0>(_pinyinCache.value())) {
        populatePinyinCache(hash);
    }

    return std::get<1>(_pinyinCache.value());
}

std::vector<int> SQLSearch::allRowIds()
{
    QSqlQuery query{_manager->getDatabase()};
    query.prepare(FETCH_ALL_ROW_ID_QUERY);
    query.setForwardOnly(true);
    query.exec();

    std::vector<int> entryIds;
    const int entryIdIndex = query.record().indexOf("entry_id");
    while (query.next()) {
        entryIds.emplace_back(query.value(entryIdIndex).toInt());
    }

    return entryIds;
}

std::vector<int> SQLSearch::rowIdFilteredByJyutping(const QString &jyutping)
{
    QSqlQuery query{_manager->getDatabase()};
    query.prepare(FETCH_ROW_ID_WITH_JYUTPING_PREFIX_QUERY);
    query.addBindValue(jyutping + "*");
    query.setForwardOnly(true);
    query.exec();

    std::vector<int> entryIds;
    const int entryIdIndex = query.record().indexOf("entry_id");
    while (query.next()) {
        entryIds.emplace_back(query.value(entryIdIndex).toInt());
    }

    return entryIds;
}

std::vector<int> SQLSearch::rowIdFilteredByPinyin(const QString &pinyin)
{
    QSqlQuery query{_manager->getDatabase()};
    query.prepare(FETCH_ROW_ID_WITH_PINYIN_PREFIX_QUERY);
    query.addBindValue(pinyin + "*");
    query.setForwardOnly(true);
    query.exec();

    std::vector<int> entryIds;
    const int entryIdIndex = query.record().indexOf("entry_id");
    while (query.next()) {
        entryIds.emplace_back(query.value(entryIdIndex).toInt());
    }

    return entryIds;
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
    QFutureWatcher<void> *watcher = new QFutureWatcher<void>();
    QObject::connect(watcher,
                     &QFutureWatcher<void>::finished,
                     watcher,
                     [this, watcher] {
                         {
                             std::lock_guard lock(_watchers.mut);
                             _watchers.set.erase(watcher);
                         }
                         delete watcher;
                     });
    {
        std::lock_guard lock(_watchers.mut);
        _watchers.set.emplace(watcher);
    }
    watcher->setFuture(
        QtConcurrent::run(&SQLSearch::searchByUniqueThread,
                          this,
                          simplified.normalized(QString::NormalizationForm_C),
                          traditional.normalized(QString::NormalizationForm_C),
                          jyutping.normalized(QString::NormalizationForm_C),
                          pinyin.normalized(QString::NormalizationForm_C),
                          queryID));
}

void SQLSearch::searchTraditionalExamples(const QString &searchTerm)
{
    unsigned long long queryID = generateAndSetQueryID();
    runThread(&SQLSearch::searchTraditionalExamplesThread,
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

    QFutureWatcher<void> *watcher = new QFutureWatcher<void>();
    QObject::connect(watcher,
                     &QFutureWatcher<void>::finished,
                     watcher,
                     [this, watcher] {
                         {
                             std::lock_guard lock(_watchers.mut);
                             _watchers.set.erase(watcher);
                         }
                         delete watcher;
                     });
    {
        std::lock_guard lock(_watchers.mut);
        _watchers.set.emplace(watcher);
    }
    watcher->setFuture(
        QtConcurrent::run(threadFunction, this, searchTerm, queryID));
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
    query.setForwardOnly(true);
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

    QString pattern;
    prepareJyutpingBindValues(searchTerm,
                              pattern,
                              fuzzyJyutping,
                              unsafeFuzzyJyutping);

    if (!fuzzyJyutping) {
        query.prepare(SEARCH_JYUTPING_QUERY);
        query.addBindValue(pattern);
        query.setForwardOnly(true);
        query.exec();
    } else {
        std::vector<QString> prefixes = literalPrefixes(pattern.sliced(1));
        std::unordered_set<int> filteredRowIds;
        if (prefixes.empty()) {
            std::vector<int> temp{allRowIds()};
            filteredRowIds.insert(temp.cbegin(), temp.cend());
        } else {
            for (const auto &prefix : prefixes) {
                std::vector<int> temp{rowIdFilteredByJyutping(prefix)};
                filteredRowIds.insert(temp.cbegin(), temp.cend());
            }
        }

        QJsonArray arr;
        for (const auto &x : filteredRowIds) {
            arr.append(x);
        }

        QJsonDocument doc(arr);
        QString json = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));

        query.prepare(SEARCH_FUZZY_JYUTPING_QUERY);
        query.addBindValue(json);
        query.addBindValue(pattern);
        query.setForwardOnly(true);
        query.exec();
    }
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

void SQLSearch::searchPinyinThread(const QString &searchTerm,
                                   const unsigned long long queryID)
{
    std::vector<Entry> results;

    QSqlQuery query{_manager->getDatabase()};

    bool fuzzyPinyin
        = _settings->value("Search/fuzzyPinyin", QVariant{true}).toBool();
    QString pattern;
    preparePinyinBindValues(searchTerm, pattern, fuzzyPinyin);

    if (!fuzzyPinyin) {
        query.prepare(SEARCH_PINYIN_QUERY);
        query.addBindValue(pattern);
        query.setForwardOnly(true);
        query.exec();
    } else {
        std::vector<QString> prefixes = literalPrefixes(pattern.sliced(1));
        std::unordered_set<int> filteredRowIds;
        if (prefixes.empty()) {
            std::vector<int> temp{allRowIds()};
            filteredRowIds.insert(temp.cbegin(), temp.cend());
        } else {
            for (const auto &prefix : prefixes) {
                std::vector<int> temp{rowIdFilteredByPinyin(prefix)};
                filteredRowIds.insert(temp.cbegin(), temp.cend());
            }
        }

        QJsonArray arr;
        for (const auto &x : filteredRowIds) {
            arr.append(x);
        }

        QJsonDocument doc(arr);
        QByteArray json = doc.toJson(QJsonDocument::Compact);

        query.prepare(SEARCH_FUZZY_PINYIN_QUERY);
        query.addBindValue(QString::fromUtf8(json));
        query.addBindValue(pattern);
        query.setForwardOnly(true);
        query.exec();
    }
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
    bool jyutpingExists = false;
    QString jyutpingSearchTerm;
    prepareJyutpingBindValues(searchTerm,
                              jyutpingSearchTerm,
                              fuzzyJyutping,
                              unsafeFuzzyJyutping);
    if (!fuzzyJyutping) {
        jyutpingQuery.prepare(SEARCH_JYUTPING_EXISTS_QUERY);
        jyutpingQuery.addBindValue(jyutpingSearchTerm);
        jyutpingQuery.setForwardOnly(true);
        jyutpingQuery.exec();
        jyutpingExists = QueryParseUtils::parseExistence(jyutpingQuery);
    } else {
        QRegularExpression r{jyutpingSearchTerm.sliced(1)};
        std::unordered_set<QString> filteredJyutping;
        std::vector<QString> prefixes = literalPrefixes(
            jyutpingSearchTerm.sliced(1));
        if (prefixes.empty()) {
            std::shared_ptr<std::vector<QString>> jyutping = allJyutping();
            filteredJyutping.insert(jyutping->cbegin(), jyutping->cend());
        } else {
            for (const auto &prefix : prefixes) {
                std::vector<QString> temp{jyutpingFilteredBy(prefix)};
                filteredJyutping.insert(temp.cbegin(), temp.cend());
            }
        }
        if (!checkQueryIDCurrent(queryID)) {
            return;
        }
        for (const auto &candidate : filteredJyutping) {
            if (r.match(candidate).hasMatch()) {
                jyutpingExists = true;
                break;
            }
        }
    }

    if (jyutpingExists) {
        notifyObserversIfQueryIdCurrent(SearchParameters::JYUTPING, queryID);
        searchJyutpingThread(searchTerm, queryID);
        return;
    }

    bool pinyinExists = false;
    QString pinyinSearchTerm;
    preparePinyinBindValues(searchTerm, pinyinSearchTerm, fuzzyPinyin);
    if (!fuzzyPinyin) {
        QSqlQuery pinyinQuery{_manager->getDatabase()};
        pinyinQuery.prepare(QString{SEARCH_PINYIN_EXISTS_QUERY}.arg(GLOB_STR));
        pinyinQuery.addBindValue(pinyinSearchTerm);
        pinyinQuery.setForwardOnly(true);
        pinyinQuery.exec();
        pinyinExists = QueryParseUtils::parseExistence(pinyinQuery);
    } else {
        QRegularExpression r{pinyinSearchTerm.sliced(1)};
        std::unordered_set<QString> filteredPinyin;
        std::vector<QString> prefixes = literalPrefixes(
            pinyinSearchTerm.sliced(1));
        if (prefixes.empty()) {
            std::shared_ptr<std::vector<QString>> pinyin;
            pinyin = allPinyin();
            filteredPinyin.insert(pinyin->cbegin(), pinyin->cend());
        } else {
            for (const auto &prefix : prefixes) {
                std::vector<QString> temp{pinyinFilteredBy(prefix)};
                filteredPinyin.insert(temp.cbegin(), temp.cend());
            }
        }
        if (!checkQueryIDCurrent(queryID)) {
            return;
        }
        for (const auto &candidate : filteredPinyin) {
            if (r.match(candidate).hasMatch()) {
                pinyinExists = true;
                break;
            }
        }
    }

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

// To search for examples, use the example_links table to JOIN
// between the example and example_translations tables.
void SQLSearch::searchTraditionalExamplesThread(const QString &searchTerm,
                                                const unsigned long long queryID)
{
    std::vector<Example> results;

    QSqlQuery query{_manager->getDatabase()};
    query.prepare(SEARCH_TRADITIONAL_EXAMPLES_QUERY);
    query.addBindValue("%" + searchTerm + "%");
    query.setForwardOnly(true);
    query.exec();

    if (!checkQueryIDCurrent(queryID)) {
        return;
    }
    results = QueryParseUtils::parseExamples(query);

    if (!checkQueryIDCurrent(queryID)) { return; }
    notifyObserversIfQueryIdCurrent(results, /*emptyQuery=*/false, queryID);
}

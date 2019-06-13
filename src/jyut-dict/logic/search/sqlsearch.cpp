#include "sqlsearch.h"

#include <QtConcurrent/QtConcurrent>

#ifdef Q_OS_WIN
#include <cctype>
#endif
#include <functional>
#include <sstream>

std::list<ISearchObserver *> SQLSearch::_observers;

SQLSearch::SQLSearch()
    : QObject()
{

}

SQLSearch::SQLSearch(std::shared_ptr<SQLDatabaseManager> manager)
    : QObject()
{
    _manager = manager;
    if (!_manager->isEnglishDatabaseOpen()) {
        _manager->openEnglishDatabase();
    }
}

SQLSearch::SQLSearch(const SQLSearch &search)
    : QObject()
{
    if (search._manager != nullptr) {
        _manager = search._manager;
    }
}

SQLSearch::~SQLSearch()
{

}

void SQLSearch::registerObserver(ISearchObserver *observer)
{
    SQLSearch::_observers.push_back(observer);
}

void SQLSearch::deregisterObserver(ISearchObserver *observer)
{
    SQLSearch::_observers.remove(observer);
}

void SQLSearch::notifyObservers()
{
    std::list<ISearchObserver *>::iterator it = SQLSearch::_observers.begin();
    while (it != SQLSearch::_observers.end())
    {
        (static_cast<ISearchObserver *>(*it))->callback(_results);
        ++it;
    }
}

void SQLSearch::searchSimplified(const QString &searchTerm)
{
    runThread(&SQLSearch::searchSimplifiedThread, searchTerm);
}

void SQLSearch::searchTraditional(const QString &searchTerm)
{
    runThread(&SQLSearch::searchTraditionalThread, searchTerm);
}

void SQLSearch::searchJyutping(const QString &searchTerm)
{
    runThread(&SQLSearch::searchJyutpingThread, searchTerm);
}

void SQLSearch::searchPinyin(const QString &searchTerm)
{
    runThread(&SQLSearch::searchPinyinThread, searchTerm);
}

void SQLSearch::searchEnglish(const QString &searchTerm)
{
    runThread(&SQLSearch::searchEnglishThread, searchTerm);
}

void SQLSearch::runThread(void (SQLSearch::*threadFunction)(const QString &searchTerm),
                          const QString &searchTerm)
{
    _currentSearchString = searchTerm;

    if (searchTerm.isEmpty()) {
        _results.clear();
        notifyObservers();
        return;
    }

    if (!_manager) {
        std::cout << "No database specified!" << std::endl;
        return;
    }

    QtConcurrent::run(this, threadFunction, searchTerm);
}

// For SearchSimplified and SearchTraditional, we use LIKE instead of MATCH
// even though the database is assumed to the FTS5-compatible.
// This is because FTS searches using the space as a separator, and
// Chinese words and phrases are not separated by spaces.
void SQLSearch::searchSimplifiedThread(const QString &searchTerm)
{
    std::lock_guard<std::mutex> lock(_queryMutex);
    QSqlQuery query{_manager->getEnglishDatabase()};
    query.prepare("SELECT * FROM entries WHERE simplified LIKE ? "
                  "ORDER BY freq DESC");
    query.addBindValue(searchTerm + "%");
    query.exec();

    // Check if a new query has been run
    // If so, do not parse results of previous query
    if (searchTerm != _currentSearchString) {
        return;
    }

    _results = parseEntries(query);

    // Check if a new query has been run
    // If so, do not notify observers of previous query's results
    if (searchTerm != _currentSearchString) {
        return;
    }

    notifyObservers();
}

void SQLSearch::searchTraditionalThread(const QString &searchTerm)
{
    std::lock_guard<std::mutex> lock(_queryMutex);
    QSqlQuery query{_manager->getEnglishDatabase()};
    query.prepare("SELECT * FROM entries WHERE traditional LIKE ? "
                  "ORDER BY freq DESC");
    query.addBindValue(searchTerm + "%");
    query.exec();

    // Check if a new query has been run
    // If so, do not parse results of previous query
    if (searchTerm != _currentSearchString) {
        return;
    }

    _results = parseEntries(query);

    // Check if a new query has been run
    // If so, do not notify observers of previous query's results
    if (searchTerm != _currentSearchString) {
        return;
    }

    notifyObservers();
}

// For searching jyutping and pinyin, we use MATCH and then LIKE, in order
// to take advantage of the quick full-text-search matching, before then
// filtering the results to only those that begin with the query
// using a LIKE wildcard.
//
// This approach is approximately ten times faster than simply using the LIKE
// operator and the % wildcard.
//
// !NOTE! Using QSQLQuery's positional placeholder method automatically
// surrounds the bound value with single quotes, i.e. "'". There is no need
// to add another set of quotes around placeholder values.
void SQLSearch::searchJyutpingThread(const QString &searchTerm)
{
    std::lock_guard<std::mutex> lock(_queryMutex);
    QSqlQuery query{_manager->getEnglishDatabase()};
    query.prepare("SELECT * FROM entries WHERE entries MATCH ? "
                  "AND jyutping LIKE ? "
                  "ORDER BY freq DESC");
    const char *matchJoinDelimiter = "*";
    std::string matchTerm = implodePhonetic(explodePhonetic(searchTerm, ' '),
                                            matchJoinDelimiter,
                                            /*surroundWithQuotes=*/true);
    const char *likeJoinDelimiter = "_";
    std::string likeTerm = implodePhonetic(explodePhonetic(searchTerm, ' '),
                                           likeJoinDelimiter);
    query.addBindValue("jyutping:" + QString(matchTerm.c_str()));
    query.addBindValue(QString(likeTerm.c_str()) + "%");
    query.exec();

    // Check if a new query has been run
    // If so, do not parse results of previous query
    if (searchTerm != _currentSearchString) {
        return;
    }

    _results = parseEntries(query);

    // Check if a new query has been run
    // If so, do not notify observers of previous query's results
    if (searchTerm != _currentSearchString) {
        return;
    }

    notifyObservers();
}

void SQLSearch::searchPinyinThread(const QString &searchTerm)
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

    std::lock_guard<std::mutex> lock(_queryMutex);
    QSqlQuery query{_manager->getEnglishDatabase()};
    query.prepare("SELECT * FROM entries WHERE entries MATCH ? "
                  "AND pinyin LIKE ? "
                  "ORDER BY freq DESC");
    const char *matchJoinDelimiter = "*";
    std::string matchTerm = implodePhonetic(explodePhonetic(processedSearchTerm, ' '),
                                            matchJoinDelimiter,
                                            /*surroundWithQuotes=*/true);
    const char *likeJoinDelimiter = "_";
    std::string likeTerm = implodePhonetic(explodePhonetic(processedSearchTerm, ' '),
                                           likeJoinDelimiter);
    query.addBindValue("pinyin:" + QString{matchTerm.c_str()});
    query.addBindValue(QString(likeTerm.c_str()) + "%");

    query.exec();

    // Check if a new query has been run
    // If so, do not parse results of previous query
    if (searchTerm != _currentSearchString) {
        return;
    }

    _results = parseEntries(query);

    // Check if a new query has been run
    // If so, do not notify observers of previous query's results
    if (searchTerm != _currentSearchString) {
        return;
    }

    notifyObservers();
}

// Searching English is the simplest query. It does a full-text search of the
// English columns.
void SQLSearch::searchEnglishThread(const QString &searchTerm)
{
    std::lock_guard<std::mutex> lock(_queryMutex);
    QSqlQuery query{_manager->getEnglishDatabase()};
    query.prepare("SELECT * FROM entries WHERE entries MATCH ? "
                  "OR entries MATCH ? "
                  "ORDER BY freq DESC");
    query.addBindValue("cedict_english:\"" + searchTerm + "\"");
    query.addBindValue("canto_english:\"" + searchTerm + "\"");
    query.setForwardOnly(true);

    query.exec();

    // Check if a new query has been run
    // If so, do not parse results of previous query
    if (searchTerm != _currentSearchString) {
        return;
    }

    _results = parseEntries(query);

    // Check if a new query has been run
    // If so, do not notify observers of previous query's results
    if (searchTerm != _currentSearchString) {
        return;
    }

    notifyObservers();
}

// explodePhonetic takes a string and a delimiter, then separates that string up
// into its components as delimited by, you guessed it, the delimiter.
// Similar to the .split() function in Python and JavaScript.
std::vector<std::string> SQLSearch::explodePhonetic(const QString &string,
                                                    const char delimiter)
{
    std::vector<std::string> words;
    std::stringstream ss(string.toStdString());
    std::string word;

    while (std::getline(ss, word, delimiter)) {
        words.push_back(word);
    }

    return words;
}

// implodePhonetic does the opposite of explodePhonetic, i.e. it takes a vector
// of strings and stitches them together with a delimiter.
//
// Since this is used for searching, we check whether to add a single wildcard
// character at the end of each string: if the last character of the string
// is a number, then we do not add a wildcard, otherwise, we do.
//
// The reason for this is as follows: when searching via romanization systems,
// it can be assumed that adding a digit to the end of a word "terminates" it,
// as it represents a tone. Without a digit, the user may not have completed
// typing the word or neglected to type a tone.
//
// So by adding a single wildcard character, we match against any word that
// 1) has that pronunciation in any of the tones, or
// 2) (if one word) any word with at least one more character following the word
// 3) (if multiple words) any words with any/specific tones, except for the last
//    word, which matches against all words that start with that spelling.
//
// In addition, we also allow surrounding each search term with quotes.
// For SQLite3's FTS, used for MATCH, this indicates that the enclosed term is a
// string. In order for the search to work correcty, the wildcard character
// (or, as it is called in FTS's documentation, the "prefix token") must be
// placed outside of the string. e.g. "ke"* is correct, whereas "ke*" is not.
//
// Example 1 - Single word:
// Searching jyutping with "se" does two calls to implodePhonetic:
// 1) For MATCH, the delimiter is "*" and surroundWithQuotes is set to true.
//    The phrase is affixed with the prefix token "*", to select all
//    words/phrases that begin with "se". The return value is,
//    including the quotes and star, "se"*.
// 2) For LIKE, the phrase is affixed with the wildcard character "_",
//    which allows it to be matched with "se1, se2, se3, se4, se5, se6".
//    The return value is se_.
//
//    Currently, since searchJyutping also appends the unlimited wildcard "%"
//    at the end of the query, it would also match against "sei1" or "seoi5" or
//    any other word or phrase that starts with "se" and contains at least one
//    more character following the word.
//
// Example 2 - Multiple words, no tone marker:
// Searching jyutping with "daai koi" is first exploded into "daai" and "koi"
// by explodePhonetic, then does two calls to implodePhonetic:
// 1) For MATCH, each phrase is affixed with the prefix token. The return
//    value is "daai"* koi"*.
// 2) For LIKE, each phrase is affixed with the single character wildcard.
//    The return value is daai_ koi_.
//
// Example 3 - Multiple words, some tone markers:
// Searching pinyin with "ke3 ai" is first exploded into "ke3" and "ai" by
// explodePhonetic, then does two calls to implodePhonetic:
// 1) For MATCH, since the first phrase ends with a digit, it is not affixed
//    with the prefix token (as the presence of a digit implies that it is
//    "complete"). The second phrase, without a digit, is affixed with a token.
//    The return value is thus "ke3" "ai"*.
// 2) For LIKE, the first phrase is not affixed with a single character
//    wildcard, as it is terminated by a digit. The second one is not, so it
//    is affixed with the single character wildcard. The return value is
//    ke3 ai_.
std::string SQLSearch::implodePhonetic(std::vector<std::string> words,
                                       const char *delimiter,
                                       bool surroundWithQuotes)
{
    const char *quotes = surroundWithQuotes ? "\"": "";
    std::ostringstream string;
    for (size_t i = 0; i < words.size() - 1; i++) {
        if (std::isdigit(words[i].back())) {
            string << quotes << words[i] << quotes << " ";
        } else {
            string << quotes << words[i] << quotes << delimiter << " ";
        }
    }

    if (std::isdigit(words.back().back())) {
        string << quotes << words.back() << quotes;
    } else {
        string << quotes << words.back() << quotes << delimiter;
    }
    return string.str();
}

std::vector<Entry> SQLSearch::parseEntries(QSqlQuery &query)
{
    std::vector<Entry> entries;

    int simplifiedIndex = query.record().indexOf("simplified");
    int traditionalIndex = query.record().indexOf("traditional");
    int jyutpingIndex = query.record().indexOf("jyutping");
    int pinyinIndex = query.record().indexOf("pinyin");
    int cedictIndex = query.record().indexOf("cedict_english");
    int cccantoIndex = query.record().indexOf("canto_english");
//    int derivedWordsIndex = query.record().indexOf("derivedWords");
//    int sentencesIndex = query.record().indexOf("sentences");
    while (query.next())
    {
         std::string simplified = query.value(simplifiedIndex).toString().toStdString();

         std::string traditional = query.value(traditionalIndex).toString().toStdString();

         std::string jyutping = query.value(jyutpingIndex).toString().toStdString();

         std::string pinyin = query.value(pinyinIndex).toString().toStdString();

         std::vector <DefinitionsSet> definitionsSets{};

         std::string cedictDefinitions = query.value(cedictIndex).toString().toStdString();
         if (!cedictDefinitions.empty()) {
            DefinitionsSet cedict_set = DefinitionsSet(CEDICT, cedictDefinitions);
            definitionsSets.push_back(cedict_set);
         }

         std::string cccantoDefinitions = query.value(cccantoIndex).toString().toStdString();
         if (!cccantoDefinitions.empty()) {
            DefinitionsSet canto_set = DefinitionsSet(CCCANTO, cccantoDefinitions);
            definitionsSets.push_back(canto_set);
         }

         entries.push_back(Entry(simplified, traditional,
                                 jyutping, pinyin,
                                 definitionsSets,
                                 std::vector<std::string>{},
                                 std::vector<Sentence>{}));
    }

    return entries;
}

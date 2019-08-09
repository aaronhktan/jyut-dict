#include "sqlsearch.h"

#include "logic/utils/utils.h"

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
    if (!_manager->isDatabaseOpen()) {
        _manager->openDatabase();
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
// even though the database is FTS5-compatible.
// This is because FTS searches using the space as a separator, and
// Chinese words and phrases are not separated by spaces.
void SQLSearch::searchSimplifiedThread(const QString &searchTerm)
{
    std::lock_guard<std::mutex> lock(_queryMutex);
    QSqlQuery query{_manager->getDatabase()};
    query.prepare("SELECT traditional, simplified, pinyin, jyutping, "
                  "group_concat(sourcename || ' ' || definition, '●') AS definitions "
                  "FROM entries, definitions, sources "
                  "WHERE simplified LIKE ? "
                  "AND entry_id = fk_entry_id "
                  "AND source_id = fk_source_id "
                  "GROUP BY entry_id "
                  "ORDER BY frequency DESC");
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
    QSqlQuery query{_manager->getDatabase()};
    query.prepare("SELECT traditional, simplified, pinyin, jyutping, "
                  "group_concat(sourcename || ' ' || definition, '●') AS definitions "
                  "FROM entries, definitions, sources "
                  "WHERE traditional LIKE ? "
                  "AND entry_id = fk_entry_id "
                  "AND source_id = fk_source_id "
                  "GROUP BY entry_id "
                  "ORDER BY frequency DESC");
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
    QSqlQuery query{_manager->getDatabase()};
    query.prepare("SELECT traditional, simplified, pinyin, jyutping, "
                  "group_concat(sourcename || ' ' || definition, '●') AS definitions "
                  "FROM entries, definitions, sources "
                  "WHERE entry_id IN "
                  "(SELECT rowid FROM entries_fts WHERE entries_fts MATCH ?"
                  " AND jyutping LIKE ?) "
                  "AND entry_id = fk_entry_id "
                  "AND source_id = fk_source_id "
                  "GROUP BY entry_id "
                  "ORDER BY frequency DESC");
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
    QSqlQuery query{_manager->getDatabase()};
    query.prepare("SELECT traditional, simplified, pinyin, jyutping, "
                  "group_concat(sourcename || ' ' || definition, '●') AS definitions "
                  "FROM entries, definitions, sources "
                  "WHERE entry_id IN "
                  "(SELECT rowid FROM entries_fts WHERE entries_fts MATCH ?"
                  " AND pinyin LIKE ?) "
                  "AND entry_id = fk_entry_id "
                  "AND source_id = fk_source_id "
                  "GROUP BY entry_id "
                  "ORDER BY frequency DESC");
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

// Searching English is the most complicated query.
// First, we match against the full-text columns in definitions_fts,
// and extract the foreign key that corresponds to the entry that matches
// that definition.
// After that, we select all the definitions that match those entries,
// along with the source that accompanies that entry,
// grouping by the entry ID and merging all the definitions into one column.
//
// For some reason, using two subqueries is ~2-3x faster than doing two
// INNER JOINs. This is related in some way to the fk_entry_id index, but I'm
// not entirely sure why.
void SQLSearch::searchEnglishThread(const QString &searchTerm)
{
    std::lock_guard<std::mutex> lock(_queryMutex);
    QSqlQuery query{_manager->getDatabase()};
    query.prepare("SELECT traditional, simplified, pinyin, jyutping, "
                  "group_concat(sourcename || ' ' || definition, '●') AS definitions "
                  "FROM entries, definitions, sources "
                  "WHERE entry_id IN "
                  "(SELECT fk_entry_id FROM definitions WHERE rowid IN "
                  "    (SELECT rowid FROM definitions_fts WHERE definitions_fts MATCH ?) "
                  ") "
                  "AND entry_id = fk_entry_id "
                  "AND source_id = fk_source_id "
                  "GROUP BY entry_id "
                  "ORDER BY frequency DESC");
    query.addBindValue("\"" + searchTerm + "\"");
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
    int definitionIndex = query.record().indexOf("definitions");

    while (query.next())
    {
        // Get fields from table
        std::string simplified = query.value(simplifiedIndex).toString().toStdString();
        std::string traditional = query.value(traditionalIndex).toString().toStdString();
        std::string jyutping = query.value(jyutpingIndex).toString().toStdString();
        std::string pinyin = query.value(pinyinIndex).toString().toStdString();
        std::string definition = query.value(definitionIndex).toString().toStdString();
        if (definition.empty()) {
            continue;
        }

        // Parse definitions
        std::vector<std::string> definitions;
        Utils::split(definition, "●", definitions);

        // Put definitions in the correct DefinitionsSet
        std::vector<DefinitionsSet> definitionsSets = {};
        for (std::string definition : definitions) {
            DefinitionsSet *set;
            std::string source = definition.substr(0,
                                                   definition.find_first_of(
                                                       " "));

            // Search definitionsSets for a matching set (i.e. set with same source)
            // Create a new DefinitionsSet for set if it no matches found
            // Then get a handle on that set
            auto search = std::find_if(definitionsSets.begin(),
                                       definitionsSets.end(),
                                       [source,
                                        definition](const DefinitionsSet &set) {
                                           return set.getSource() == source;
                                       });
            if (search == definitionsSets.end()) {
                definitionsSets.push_back(DefinitionsSet{source});
                set = &definitionsSets.back();
            } else {
                set = &*search;
            }

            // Push the definition to that set
            std::string definitionContent = definition.substr(
                definition.find_first_of(" ") + 1);

            set->pushDefinition(definitionContent);
        }

        entries.push_back(Entry(simplified,
                                traditional,
                                jyutping,
                                pinyin,
                                definitionsSets,
                                std::vector<std::string>{},
                                std::vector<Sentence>{}));
    }

    return entries;
}

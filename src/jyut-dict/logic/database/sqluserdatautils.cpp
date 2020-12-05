#include "sqluserdatautils.h"

#include "logic/utils/utils.h"

#include <QtConcurrent/QtConcurrent>

SQLUserDataUtils::SQLUserDataUtils(std::shared_ptr<SQLDatabaseManager> manager)
    : _manager{manager}
{
}

SQLUserDataUtils::~SQLUserDataUtils() {}

void SQLUserDataUtils::registerObserver(ISearchObserver *observer)
{
    _observers.push_back(observer);
}

void SQLUserDataUtils::deregisterObserver(ISearchObserver *observer)
{
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

void SQLUserDataUtils::notifyObservers(bool entryExists, Entry entry)
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
    QtConcurrent::run(this,
                      &SQLUserDataUtils::searchForAllFavouritedWordsThread);
}

void SQLUserDataUtils::checkIfEntryHasBeenFavourited(Entry entry)
{
    if (!_manager) {
        std::cout << "No database specified!" << std::endl;
        return;
    }
    QtConcurrent::run(this,
                      &SQLUserDataUtils::checkIfEntryHasBeenFavouritedThread,
                      entry);
}

void SQLUserDataUtils::favouriteEntry(Entry entry)
{
    if (!_manager) {
        std::cout << "No database specified!" << std::endl;
        return;
    }
    QtConcurrent::run(this,
                      &SQLUserDataUtils::favouriteEntryThread,
                      entry);
}

void SQLUserDataUtils::unfavouriteEntry(Entry entry)
{
    if (!_manager) {
        std::cout << "No database specified!" << std::endl;
        return;
    }
    QtConcurrent::run(this,
                      &SQLUserDataUtils::unfavouriteEntryThread,
                      entry);
}

void SQLUserDataUtils::searchForAllFavouritedWordsThread(void)
{
    std::vector<Entry> results{};

    {
        std::lock_guard<std::mutex> databaseLock(_databaseMutex);
        QSqlQuery query{_manager->getDatabase()};

        query.exec("DROP TABLE IF EXISTS favourite_tmp");
        // Create a table with two columns, the entry_id and its associated
        // timestamp.
        query.exec("CREATE TEMPORARY TABLE favourite_tmp AS "
                   "SELECT entry_id, timestamp from entries "
                   "INNER JOIN user.favourite_words "
                   " ON entries.simplified = favourite_words.simplified "
                   " AND entries.traditional = favourite_words.traditional "
                   " AND entries.jyutping = favourite_words.jyutping "
                   " AND entries.pinyin = favourite_words.pinyin");
        // Select the fields of the entry and its definitions from the entry_id
        // from the temporary table, then sort by the timestamp.
        query.exec(
            "SELECT entries.traditional, entries.simplified, entries.pinyin, "
            " entries.jyutping, "
            " group_concat(sourcename || ' ' || definition, '●') AS "
            " definitions, timestamp "
            "FROM entries, definitions, sources, favourite_tmp "
            "WHERE entries.entry_id IN "
            "(SELECT entry_id FROM favourite_tmp) "
            "AND favourite_tmp.entry_id = entries.entry_id "
            "AND entries.entry_id = fk_entry_id "
            "AND source_id = fk_source_id "
            "GROUP BY entries.entry_id "
            "ORDER BY timestamp DESC");

        results = parseEntries(query);
        _manager->closeDatabase();
    }

    notifyObservers(results, /*emptyQuery=*/false);
}

void SQLUserDataUtils::checkIfEntryHasBeenFavouritedThread(Entry entry)
{
    bool existence = false;
    {
        std::lock_guard<std::mutex> databaseLock(_databaseMutex);
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
        existence = parseExistence(query);
        _manager->closeDatabase();
    }

    notifyObservers(existence, entry);
}

void SQLUserDataUtils::favouriteEntryThread(Entry entry)
{
    {
        std::lock_guard<std::mutex> databaseLock(_databaseMutex);
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
        _manager->closeDatabase();
    }

    checkIfEntryHasBeenFavourited(entry);
    searchForAllFavouritedWords();
}

void SQLUserDataUtils::unfavouriteEntryThread(Entry entry)
{
    {
        std::lock_guard<std::mutex> databaseLock(_databaseMutex);
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
        _manager->closeDatabase();
    }

    checkIfEntryHasBeenFavourited(entry);
    searchForAllFavouritedWords();
}

std::vector<Entry> SQLUserDataUtils::parseEntries(QSqlQuery &query)
{
    std::vector<Entry> entries;

    int simplifiedIndex = query.record().indexOf("simplified");
    int traditionalIndex = query.record().indexOf("traditional");
    int jyutpingIndex = query.record().indexOf("jyutping");
    int pinyinIndex = query.record().indexOf("pinyin");
    int definitionIndex = query.record().indexOf("definitions");

    while (query.next()) {
        // Get fields from table
        std::string simplified
            = query.value(simplifiedIndex).toString().toStdString();
        std::string traditional
            = query.value(traditionalIndex).toString().toStdString();
        std::string jyutping
            = query.value(jyutpingIndex).toString().toStdString();
        std::string pinyin = query.value(pinyinIndex).toString().toStdString();
        std::string definition
            = query.value(definitionIndex).toString().toStdString();
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
                                std::vector<SourceSentence>{}));
    }

    return entries;
}

bool SQLUserDataUtils::parseExistence(QSqlQuery &query)
{
    bool existence = 0;

    int existenceIndex = query.record().indexOf("existence");

    while (query.next()) {
        existence = query.value(existenceIndex).toInt() == 1;
    }

    return existence;
}

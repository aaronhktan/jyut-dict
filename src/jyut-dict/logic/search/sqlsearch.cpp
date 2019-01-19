#include "sqlsearch.h"

#include <QFile>

#include <sstream>

std::list<ISearchObserver *> SQLSearch::_observers;
SQLDatabaseManager *SQLSearch::_manager;

SQLSearch::SQLSearch()
{
    if (!SQLSearch::_manager) {
       SQLSearch::_manager = new SQLDatabaseManager();
    }

    if (!SQLSearch::_manager->isEnglishDatabaseOpen()) {
       SQLSearch::_manager->openEnglishDatabase();
    }
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

void SQLSearch::searchSimplified(const QString& searchTerm)
{
    if (searchTerm.isEmpty()) {
        _results.clear();
        notifyObservers();
        return;
    }

    QSqlQuery query{SQLSearch::_manager->getEnglishDatabase()};
    query.prepare("SELECT * FROM entries WHERE simplified LIKE ? "
                  "ORDER BY freq DESC");
    query.addBindValue(searchTerm + "%");
    query.exec();

    _results = parseEntries(query);

    notifyObservers();
}

void SQLSearch::searchTraditional(const QString& searchTerm)
{
    if (searchTerm.isEmpty()) {
        _results.clear();
        notifyObservers();
        return;
    }

    QSqlQuery query{SQLSearch::_manager->getEnglishDatabase()};
    query.prepare("SELECT * FROM entries WHERE traditional LIKE ? "
                  "ORDER BY freq DESC");
    query.addBindValue(searchTerm + "%");
    query.exec();

    _results = parseEntries(query);

    notifyObservers();
}

void SQLSearch::searchJyutping(const QString &searchTerm)
{
    if (searchTerm.isEmpty()) {
        _results.clear();
        notifyObservers();
        return;
    }

    QSqlQuery query{SQLSearch::_manager->getEnglishDatabase()};
    query.prepare("SELECT * FROM entries WHERE jyutping MATCH ? "
                  "AND jyutping LIKE ? "
                  "ORDER BY freq DESC");
    const char *matchJoinDelimiter = "*";
    std::string matchTerm = implodePhonetic(explodePhonetic(searchTerm, ' '), matchJoinDelimiter);
    const char *likeJoinDelimiter = "_";
    std::string likeTerm = implodePhonetic(explodePhonetic(searchTerm, ' '), likeJoinDelimiter);
    query.addBindValue(QString(matchTerm.c_str()));
    query.addBindValue(QString(likeTerm.c_str()) + "%");
    query.exec();

    _results = parseEntries(query);

    notifyObservers();
}

void SQLSearch::searchPinyin(const QString &searchTerm)
{
    if (searchTerm.isEmpty()) {
        _results.clear();
        notifyObservers();
        return;
    }

    QSqlQuery query{SQLSearch::_manager->getEnglishDatabase()};
    query.prepare("SELECT * FROM entries WHERE pinyin MATCH ? "
                  "AND pinyin LIKE ? "
                  "ORDER BY freq DESC");
    const char *matchJoinDelimiter = "*";
    std::string matchTerm = implodePhonetic(explodePhonetic(searchTerm, ' '), matchJoinDelimiter);
    const char *likeJoinDelimiter = "_";
    std::string likeTerm = implodePhonetic(explodePhonetic(searchTerm, ' '), likeJoinDelimiter);
    query.addBindValue(QString(matchTerm.c_str()));
    query.addBindValue(QString(likeTerm.c_str()) + "%");

    query.exec();

    _results = parseEntries(query);

    notifyObservers();
}

void SQLSearch::searchEnglish(const QString& searchTerm)
{
    if (searchTerm.isEmpty()) {
        _results.clear();
        notifyObservers();
        return;
    }

    QSqlQuery query{SQLSearch::_manager->getEnglishDatabase()};
    query.prepare("SELECT * FROM entries WHERE entries MATCH ? "
                  "OR entries MATCH ? "
                  "ORDER BY freq DESC");
    query.addBindValue("cedict_english:" + searchTerm);
    query.addBindValue("canto_english:" + searchTerm);
    query.setForwardOnly(true);
    query.exec();

    _results = parseEntries(query);

    notifyObservers();
}

std::vector<std::string> SQLSearch::explodePhonetic(const QString &string, const char delimiter)
{
    std::vector<std::string> words;
    std::stringstream ss(string.toStdString());
    std::string word;

    while (std::getline(ss, word, delimiter)) {
        words.push_back(word);
    }

    return words;
}

std::string SQLSearch::implodePhonetic(std::vector<std::string> words, const char *delimiter)
{
    std::ostringstream string;
    for (size_t i = 0; i < words.size() - 1; i++) {
        if (std::isdigit(words[i].back())) {
            string << words[i] << " ";
        } else {
            string << words[i] << delimiter << " ";
        }
    }

    if (std::isdigit(words.back().back())) {
        string << words.back();
    } else {
        string << words.back() << delimiter;
    }
    return string.str();
}

std::vector<Entry> SQLSearch::parseEntries(QSqlQuery query)
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

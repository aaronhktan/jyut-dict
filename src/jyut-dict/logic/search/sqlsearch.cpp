#include "sqlsearch.h"

#include <QFile>

std::list<ISearchObserver *> SQLSearch::_observers;

SQLSearch::SQLSearch()
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

void SQLSearch::searchSimplified(const QString& searchTerm)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM entries WHERE simplified LIKE ?");
    query.addBindValue("%" + searchTerm + "%");
    query.exec();

    _results = parseEntries(query);

    notifyObservers();
}

void SQLSearch::searchTraditional(const QString& searchTerm)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM entries WHERE traditional LIKE ?");
    query.addBindValue("%" + searchTerm + "%");
    query.exec();

    _results = parseEntries(query);

    notifyObservers();
}

void SQLSearch::searchJyutping(const QString &searchTerm)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM entries WHERE jyutping LIKE ?");
    query.addBindValue(searchTerm + "%");
    query.exec();

    _results = parseEntries(query);

    notifyObservers();
}

void SQLSearch::searchPinyin(const QString &searchTerm)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM entries WHERE pinyin LIKE ?");
    query.addBindValue(searchTerm + "%");
    query.exec();

    _results = parseEntries(query);

    notifyObservers();
}

void SQLSearch::searchEnglish(const QString& searchTerm)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM entries WHERE entries MATCH ? "
                  "OR entries MATCH ?");
    query.addBindValue("cedict_english:" + searchTerm);
    query.addBindValue("canto_english:" + searchTerm);
    query.setForwardOnly(true);
    query.exec();

    _results = parseEntries(query);

    notifyObservers();
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

         std::string cedictDefinitions = query.value(cedictIndex).toString().toStdString();
         DefinitionsSet cedict_set = DefinitionsSet(CEDICT, cedictDefinitions);

         std::string cccantoDefinitions = query.value(cccantoIndex).toString().toStdString();
         DefinitionsSet canto_set = DefinitionsSet(CCCANTO, cccantoDefinitions);

         entries.push_back(Entry(simplified, traditional,
                                 jyutping, pinyin,
                                 std::vector<DefinitionsSet>{cedict_set, canto_set},
                                 std::vector<std::string>{},
                                 std::vector<Sentence>{}));
    }

    return entries;
}

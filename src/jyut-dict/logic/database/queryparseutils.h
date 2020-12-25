#ifndef QUERYPARSEUTILS_H
#define QUERYPARSEUTILS_H

#include "logic/entry/entry.h"
#include "logic/sentence/sourcesentence.h"

#include <QSqlQuery>

#include <vector>

namespace QueryParseUtils {

typedef std::pair<std::string, int> searchTermHistoryItem;

std::vector<Entry> parseEntries(QSqlQuery &query, bool parseDefinitions = true);
std::vector<SourceSentence> parseSentences(QSqlQuery &query);

bool parseExistence(QSqlQuery &query);
std::vector<searchTermHistoryItem> parseHistoryItems(QSqlQuery &query);

}

#endif // QUERYPARSEUTILS_H

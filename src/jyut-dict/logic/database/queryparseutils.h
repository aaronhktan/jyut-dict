#ifndef QUERYPARSEUTILS_H
#define QUERYPARSEUTILS_H

#include "logic/entry/entry.h"
#include "logic/sentence/sourcesentence.h"

#include <QSqlQuery>

#include <vector>

// The QueryParseUtils namespace contains static functions to parse the rows
// that are returned from a query on the database.

namespace QueryParseUtils {

using searchTermHistoryItem = std::pair<std::string, long>;

std::vector<Entry> parseEntries(QSqlQuery &query, bool parseDefinitions = true);
std::vector<SourceSentence> parseSentences(QSqlQuery &query);

bool parseExistence(QSqlQuery &query);
std::vector<searchTermHistoryItem> parseHistoryItems(QSqlQuery &query);

}

#endif // QUERYPARSEUTILS_H

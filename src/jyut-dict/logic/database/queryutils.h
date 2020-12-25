#ifndef QUERYUTILS_H
#define QUERYUTILS_H

#include "logic/entry/entry.h"
#include "logic/sentence/sourcesentence.h"

#include <QSqlQuery>

#include <vector>

namespace QueryUtils {

std::vector<Entry> parseEntries(QSqlQuery &query);
std::vector<SourceSentence> parseSentences(QSqlQuery &query);

bool parseExistence(QSqlQuery &query);

}

#endif // QUERYUTILS_H

#ifndef ISEARCH_H
#define ISEARCH_H

#include <QString>

// Interface to start a search in database

class ISearch
{
public:
    virtual ~ISearch() = default;

    virtual void searchSimplified(const QString &searchTerm) = 0;
    virtual void searchTraditional(const QString &searchTerm) = 0;
    virtual void searchJyutping(const QString &searchTerm) = 0;
    virtual void searchPinyin(const QString &searchTerm) = 0;
    virtual void searchEnglish(const QString &searchTerm) = 0;
    virtual void searchAutoDetect(const QString &searchTerm) = 0;

    virtual void searchByUnique(const QString &simplified,
                                const QString &traditional,
                                const QString &jyutping,
                                const QString &pinyin) = 0;
};

#endif // ISEARCH_H

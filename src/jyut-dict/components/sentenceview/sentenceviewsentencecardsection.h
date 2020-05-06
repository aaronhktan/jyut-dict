#ifndef SENTENCEVIEWSENTENCECARDSECTION_H
#define SENTENCEVIEWSENTENCECARDSECTION_H

#include "components/sentencecard/sentencecardwidget.h"
#include "logic/database/sqldatabasemanager.h"
#include "logic/search/isearchobserver.h"
#include "logic/search/sqlsearch.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include <unordered_map>
#include <vector>

class SentenceViewSentenceCardSection : public QWidget
{
    Q_OBJECT
public:
    explicit SentenceViewSentenceCardSection(QWidget *parent = nullptr);

    void setSourceSentence(const SourceSentence &sentence);

private:
    void setupUI(void);
    void cleanup(void);

    std::shared_ptr<SQLDatabaseManager> _manager;
    std::unique_ptr<SQLSearch> _search;
    std::vector<SourceSentence> _sentences;

    bool _calledBack = false;

    QVBoxLayout *_sentenceCardsLayout;
    std::vector<SentenceCardWidget *> _sentenceCards;

signals:
    void addingCards();
    void finishedAddingCards();
};

#endif // SENTENCEVIEWSENTENCECARDSECTION_H

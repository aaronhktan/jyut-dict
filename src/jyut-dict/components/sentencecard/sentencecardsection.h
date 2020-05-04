#ifndef SENTENCECARDSECTION_H
#define SENTENCECARDSECTION_H

#include "components/sentencecard/loadingwidget.h"
#include "components/sentencecard/sentencecardwidget.h"
#include "logic/database/sqldatabasemanager.h"
#include "logic/search/isearchobserver.h"
#include "logic/search/sqlsearch.h"

#include <QVBoxLayout>
#include <QWidget>

#include <unordered_map>
#include <vector>

class SentenceCardSection : public QWidget, public ISearchObserver
{
    Q_OBJECT
public:
    explicit SentenceCardSection(std::shared_ptr<SQLDatabaseManager> manager,
                                 QWidget *parent = nullptr);
    void callback(const std::vector<Entry> sourceSentences,
                  bool emptyQuery) override;
    void callback(const std::vector<SourceSentence> sourceSentences,
                  bool emptyQuery) override;

    void setEntry(const Entry &entry);

private:
    void setupUI(void);
    void cleanup(void);

    void showLoadingWidget(void);

    std::unordered_map<std::string, std::vector<SourceSentence>>
    getSamplesForEachSource(const std::vector<SourceSentence> &sourceSentences);

    std::shared_ptr<SQLDatabaseManager> _manager;
    std::unique_ptr<SQLSearch> _search;

    bool _calledBack = false;

    QVBoxLayout *_sentenceCardsLayout;
    LoadingWidget *_loadingWidget;
    std::vector<SentenceCardWidget *> _sentenceCards;

public slots:
    void updateUI(std::vector<SourceSentence> sourceSentences);

signals:
    void callbackInvoked(std::vector<SourceSentence> sourceSentences);
};

#endif // SENTENCECARDSECTION_H

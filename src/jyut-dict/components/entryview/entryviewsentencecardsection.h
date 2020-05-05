#ifndef ENTRYVIEWSENTENCECARDSECTION_H
#define ENTRYVIEWSENTENCECARDSECTION_H

#include "components/sentencecard/loadingwidget.h"
#include "components/sentencecard/sentencecardwidget.h"
#include "components/sentencewindow/sentencesplitter.h"
#include "logic/database/sqldatabasemanager.h"
#include "logic/search/isearchobserver.h"
#include "logic/search/sqlsearch.h"

#include <QString>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

#include <unordered_map>
#include <vector>

class EntryViewSentenceCardSection : public QWidget, public ISearchObserver
{
    Q_OBJECT
public:
    explicit EntryViewSentenceCardSection(std::shared_ptr<SQLDatabaseManager> manager,
                                 QWidget *parent = nullptr);
    explicit EntryViewSentenceCardSection(QWidget *parent = nullptr);
    ~EntryViewSentenceCardSection() override;
    void callback(const std::vector<Entry> sourceSentences,
                  bool emptyQuery) override;
    void callback(const std::vector<SourceSentence> sourceSentences,
                  bool emptyQuery) override;

    void setEntry(const Entry &entry);

private:
    void setupUI(void);
    void cleanup(void);

    void showLoadingWidget(void);
    void openSentenceWindow(std::vector<SourceSentence> sourceSentences);

    std::unordered_map<std::string, std::vector<SourceSentence>>
    getSamplesForEachSource(const std::vector<SourceSentence> &sourceSentences);

    std::shared_ptr<SQLDatabaseManager> _manager;
    std::unique_ptr<SQLSearch> _search;
    std::vector<SourceSentence> _sentences;
    QString _title;

    bool _calledBack = false;

    QVBoxLayout *_sentenceCardsLayout;
    LoadingWidget *_loadingWidget;
    std::vector<SentenceCardWidget *> _sentenceCards;
    QToolButton *_viewAllSentencesButton;

public slots:
    void updateUI(std::vector<SourceSentence> sourceSentences);

signals:
    void callbackInvoked(std::vector<SourceSentence> sourceSentences);
    void addingCards();
    void finishedAddingCards();
};

#endif // ENTRYVIEWSENTENCECARDSECTION_H

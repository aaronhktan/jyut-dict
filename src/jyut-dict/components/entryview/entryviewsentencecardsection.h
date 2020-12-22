#ifndef ENTRYVIEWSENTENCECARDSECTION_H
#define ENTRYVIEWSENTENCECARDSECTION_H

#include "components/sentencecard/loadingwidget.h"
#include "components/sentencecard/sentencecardwidget.h"
#include "components/sentencewindow/sentencesplitter.h"
#include "logic/database/sqldatabasemanager.h"
#include "logic/database/sqluserdatautils.h"
#include "logic/search/isearchobserver.h"
#include "logic/search/sqlsearch.h"

#include <QEvent>
#include <QString>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

#include <memory>
#include <unordered_map>
#include <vector>

// The EntryViewSentenceCardSection displays cards for each set of sentences,
// where each set of sentences belongs to a particular source.

typedef std::unordered_map<std::string, std::vector<SourceSentence>>
    sentenceSamples;

class EntryViewSentenceCardSection : public QWidget, public ISearchObserver
{
    Q_OBJECT
public:
    explicit EntryViewSentenceCardSection(std::shared_ptr<SQLDatabaseManager> manager,
                                 QWidget *parent = nullptr);
    explicit EntryViewSentenceCardSection(QWidget *parent = nullptr);
    ~EntryViewSentenceCardSection() override;
    void callback(const std::vector<SourceSentence> sourceSentences,
                  bool emptyQuery) override;

    void changeEvent(QEvent *event) override;

    void setEntry(const Entry &entry);

private:
    void setupUI(void);
    void translateUI(void);
    void cleanup(void);
    void setStyle(bool use_dark);

    void showLoadingWidget(void);
    void openSentenceWindow(std::vector<SourceSentence> sourceSentences);

    void pauseBeforeUpdatingUI(std::vector<SourceSentence> sourceSentences,
                               sentenceSamples samples);

    std::mutex layoutMutex;
    std::mutex updateMutex;

    std::unordered_map<std::string, std::vector<SourceSentence>>
    getSamplesForEachSource(const std::vector<SourceSentence> &sourceSentences);

    std::shared_ptr<SQLDatabaseManager> _manager;
    std::unique_ptr<SQLSearch> _search;
    std::vector<SourceSentence> _sentences;
    QString _title;

    bool _paletteRecentlyChanged = false;
    bool _calledBack = false;
    QTimer *_timer;
#ifdef Q_OS_WIN
    QTimer *_enableUIUpdateTimer;
    QTimer *_updateUITimer;
    bool _enableUIUpdate = false;
#endif

    QVBoxLayout *_sentenceCardsLayout;
    LoadingWidget *_loadingWidget;
    std::vector<SentenceCardWidget *> _sentenceCards;
    QToolButton *_viewAllSentencesButton;

public slots:
    void updateUI(
        std::vector<SourceSentence> sourceSentences,
        sentenceSamples samples);

#ifdef Q_OS_WIN
    void stallUIUpdate(void);
#endif

signals:
    void callbackInvoked(
        std::vector<SourceSentence> sourceSentences,
        sentenceSamples samples);
    void addingCards();
    void finishedAddingCards();
};

Q_DECLARE_METATYPE(sentenceSamples);

#endif // ENTRYVIEWSENTENCECARDSECTION_H

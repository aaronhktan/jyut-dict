#ifndef ENTRYVIEWSENTENCECARDSECTION_H
#define ENTRYVIEWSENTENCECARDSECTION_H

#include "components/sentencecard/loadingwidget.h"
#include "components/sentencecard/sentencecardwidget.h"
#include "logic/database/sqldatabasemanager.h"
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

using sentenceSamples
    = std::unordered_map<std::string, std::vector<SourceSentence>>;

class EntryViewSentenceCardSection : public QWidget, public ISearchObserver
{
    Q_OBJECT
public:
    explicit EntryViewSentenceCardSection(std::shared_ptr<SQLDatabaseManager> manager,
                                 QWidget *parent = nullptr);
    explicit EntryViewSentenceCardSection(QWidget *parent = nullptr);
    void callback(const std::vector<SourceSentence> &sourceSentences,
                  bool emptyQuery) override;

    void changeEvent(QEvent *event) override;

    void setEntry(const Entry &entry);

private:
    void setupUI(void);
    void translateUI(void);
    void cleanup(void);
    void setStyle(bool use_dark);

    void showLoadingWidget(void);
    void openSentenceWindow(const std::vector<SourceSentence> &sourceSentences);

    std::mutex layoutMutex;
    std::mutex updateMutex;

    std::unordered_map<std::string, std::vector<SourceSentence>>
    getSamplesForEachSource(const std::vector<SourceSentence> &sourceSentences) const;

    std::shared_ptr<SQLDatabaseManager> _manager;
    std::unique_ptr<SQLSearch> _search;
    std::unique_ptr<QSettings> _settings;
    std::vector<SourceSentence> _sentences;
    QString _title;

    bool _paletteRecentlyChanged = false;
    bool _calledBack = false;
    QTimer *_showLoadingIconTimer;
    QTimer *_enableUIUpdateTimer;
    QTimer *_updateUITimer;
    bool _enableUIUpdate = false;

    QVBoxLayout *_sentenceCardsLayout;
    LoadingWidget *_loadingWidget;
    std::vector<SentenceCardWidget *> _sentenceCards;
    QToolButton *_viewAllSentencesButton;

signals:
    void callbackInvoked(const std::vector<SourceSentence> &sourceSentences,
                         const sentenceSamples &samples);
    void addingCards();
    void finishedAddingCards();
    void noCardsAdded();

public slots:
    void updateUI(
        const std::vector<SourceSentence> &sourceSentences,
        const sentenceSamples &samples);
    void stallSentenceUIUpdate(void);
    void updateStyleRequested(void);
    void viewAllSentencesRequested(void);

private slots:
    void pauseBeforeUpdatingUI(const std::vector<SourceSentence> &sourceSentences,
                               const sentenceSamples &samples);
};

Q_DECLARE_METATYPE(sentenceSamples);

#endif // ENTRYVIEWSENTENCECARDSECTION_H

#ifndef ENTRYVIEWEXAMPLECARDSECTION_H
#define ENTRYVIEWEXAMPLECARDSECTION_H

#include "logic/search/isearchobserver.h"
#include "logic/search/sqlsearch.h"

#include <QSettings>
#include <QWidget>

#include <memory>
#include <unordered_map>
#include <vector>

class LoadingWidget;
class ExampleCardWidget;
class SQLDatabaseManager;

class QEvent;
class QString;
class QTimer;
class QToolButton;
class QVBoxLayout;

// The EntryViewExampleCardSection displays cards for each set of examples,
// where each set of examples belongs to a particular source.

using exampleSamples = std::unordered_map<std::string, std::vector<Example>>;

class EntryViewExampleCardSection : public QWidget, public ISearchObserver
{
    Q_OBJECT
public:
    explicit EntryViewExampleCardSection(
        std::shared_ptr<SQLDatabaseManager> manager, QWidget *parent = nullptr);
    explicit EntryViewExampleCardSection(QWidget *parent = nullptr);
    void callback(const std::vector<Example> &examples,
                  bool emptyQuery) override;

    void changeEvent(QEvent *event) override;

    void setEntry(const Entry &entry);

private:
    void setupUI(void);
    void translateUI(void);
    void cleanup(void);
    void setStyle(bool use_dark);

    void showLoadingWidget(void);
    void openExampleWindow(const std::vector<Example> &examples);

    std::mutex layoutMutex;
    std::mutex updateMutex;

    std::unordered_map<std::string, std::vector<Example>> getSamplesForEachSource(
        const std::vector<Example> &examples) const;

    std::shared_ptr<SQLDatabaseManager> _manager;
    std::unique_ptr<SQLSearch> _search;
    std::unique_ptr<QSettings> _settings;
    std::vector<Example> _examples;
    QString _title;

    bool _paletteRecentlyChanged = false;
    bool _calledBack = false;
    QTimer *_showLoadingIconTimer;
    QTimer *_enableUIUpdateTimer;
    QTimer *_updateUITimer;
    bool _enableUIUpdate = false;

    QVBoxLayout *_exampleCardsLayout;
    LoadingWidget *_loadingWidget;
    std::vector<ExampleCardWidget *> _exampleCards;
    QToolButton *_viewAllExamplesButton;

signals:
    void callbackInvoked(const std::vector<Example> &examples,
                         const exampleSamples &samples);
    void addingCards();
    void finishedAddingCards();
    void noCardsAdded();

public slots:
    void updateUI(const std::vector<Example> &examples,
                  const exampleSamples &samples);
    void stallExampleUIUpdate(void);
    void updateStyleRequested(void);
    void viewAllExamplesRequested(void);

private slots:
    void pauseBeforeUpdatingUI(const std::vector<Example> &examples,
                               const exampleSamples &samples);
};

Q_DECLARE_METATYPE(exampleSamples);

#endif // ENTRYVIEWEXAMPLECARDSECTION_H

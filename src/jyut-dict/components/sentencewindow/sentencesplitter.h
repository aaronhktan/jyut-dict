#ifndef SENTENCESPLITTER_H
#define SENTENCESPLITTER_H

#include "components/sentenceview/sentencescrollarea.h"
#include "logic/database/sqldatabasemanager.h"
#include "logic/search/sqlsearch.h"
#include "logic/sentence/sourcesentence.h"

#include <QAbstractListModel>
#include <QEvent>
#include <QModelIndex>
#include <QSplitter>
#include <QWidget>

// The SentenceSplitter contains a "master" listview and a "detail" scrollarea
//
// It handles the model changed signal that the master listview emits,
// and passes the data to the detail scrollarea.
//
// It also handles updating the model for the listview.

class SentenceSplitter : public QSplitter
{
Q_OBJECT

public:
    explicit SentenceSplitter(std::shared_ptr<SQLDatabaseManager> manager,
                              QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    void setSourceSentences(const std::vector<SourceSentence> &sourceSentences);
    void setSearchTerm(const QString &searchTerm);

private:
    void translateUI(void);

    void prepareSourceSentence(SourceSentence &sentence) const;

#ifdef Q_OS_WIN
    void setStyle(bool use_dark);
    bool _paletteRecentlyChanged = false;
#endif

    void openCurrentSelectionInNewWindow(void);

    std::shared_ptr<SQLDatabaseManager> _manager;
    std::shared_ptr<SQLSearch> _sqlSearch;

    QString _searchTerm = "";
    int _size = 0;

    SentenceScrollArea *_sentenceScrollArea;
    QAbstractListModel *_model;
    QListView *_resultListView;

private slots:
    void handleClick(const QModelIndex &selection);
    void handleDoubleClick(const QModelIndex &selection);

public slots:
    void updateStyleRequested(void);
};

#endif // SENTENCESPLITTER_H

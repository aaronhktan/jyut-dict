#ifndef EXAMPLESPLITTER_H
#define EXAMPLESPLITTER_H

#include <QSplitter>

#include <span>

class Example;
class ExampleScrollArea;
class SQLDatabaseManager;
class SQLSearch;

class QAbstractListModel;
class QEvent;
class QKeyEvent;
class QListView;
class QModelIndex;

// The ExampleSplitter contains a "content" listview and a "detail" scrollarea
//
// It handles the model changed signal that the content listview emits,
// and passes the data to the detail scrollarea.
//
// It also handles updating the model for the listview.

class ExampleSplitter : public QSplitter
{
    Q_OBJECT
public:
    explicit ExampleSplitter(std::shared_ptr<SQLDatabaseManager> manager,
                             QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    void setExamples(std::span<const Example> examples);
    void setSearchTerm(const QString &searchTerm);

private:
    void translateUI(void);

    void prepareExample(Example &example) const;

#ifdef Q_OS_WIN
    void setStyle(bool use_dark);
    bool _paletteRecentlyChanged = false;
#endif

    void openCurrentSelectionInNewWindow(void);

    std::shared_ptr<SQLDatabaseManager> _manager;
    std::shared_ptr<SQLSearch> _sqlSearch;

    QString _searchTerm = "";
    int _size = 0;

    ExampleScrollArea *_exampleScrollArea;
    QAbstractListModel *_model;
    QListView *_resultListView;

private slots:
    void handleClick(const QModelIndex &selection);
    void handleDoubleClick(const QModelIndex &selection);

public slots:
    void updateStyleRequested(void);
};

#endif // EXAMPLESPLITTER_H

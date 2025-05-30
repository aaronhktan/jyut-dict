#ifndef SEARCHLINEEDIT_H
#define SEARCHLINEEDIT_H

#include "components/mainwindow/isearchlineedit.h"
#include "logic/database/sqluserhistoryutils.h"
#include "logic/search/isearch.h"
#include "logic/search/isearchoptionsmediator.h"
#include "windows/handwritingwindow.h"
#ifndef Q_OS_LINUX
#include "windows/transcriptionwindow.h"
#endif

#include <QAction>
#include <QEvent>
#include <QFocusEvent>
#include <QLineEdit>
#include <QSettings>
#include <QTimer>
#include <QWidget>

#include <memory>

// The SearchLineEdit is the main search bar

class SearchLineEdit : public QLineEdit, public ISearchLineEdit
{
    Q_OBJECT

public:
    explicit SearchLineEdit(std::shared_ptr<ISearchOptionsMediator> mediator,
                            std::shared_ptr<ISearch> manager,
                            std::shared_ptr<SQLUserHistoryUtils> sqlHistoryUtils,
                            QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

    void updateParameters(SearchParameters parameters) override;
    void search(std::optional<SearchParameters>
                = std::optional<SearchParameters>{}) override;

private:
    void setupUI(void);
    void translateUI(void);
    void setStyle(bool use_dark);

    void checkClearVisibility(void);

    void startHandwriting(void);
#ifndef Q_OS_LINUX
    void startTranscription(void);
#endif

    void addSearchTermToHistory(SearchParameters parameters) const;

    std::shared_ptr<ISearchOptionsMediator> _mediator;
    std::shared_ptr<ISearch> _search;
    std::shared_ptr<SQLUserHistoryUtils> _sqlHistoryUtils;
    std::unique_ptr<QSettings> _settings;

    QAction *_searchLineEdit;
    QAction *_clearLineEdit;
    QAction *_handwriting;
#ifndef Q_OS_LINUX
    QAction *_microphone;
#endif
    QTimer *_timer;

    HandwritingWindow *_handwritingWindow = nullptr;
#ifndef Q_OS_LINUX
    TranscriptionWindow *_transcriptionWindow = nullptr;
#endif

    SearchParameters _parameters;

    bool _paletteRecentlyChanged = false;

public slots:
    void searchTriggered(void);

#ifndef Q_OS_LINUX
    void dictationRequested(void);
#endif
    void handwritingRequested(void);
};

#endif // SEARCHLINEEDIT_H

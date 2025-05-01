#ifndef SEARCHLINEEDIT_H
#define SEARCHLINEEDIT_H

#include "components/mainwindow/isearchlineedit.h"
#include "logic/database/sqluserhistoryutils.h"
#include "logic/dictation/itranscriptionresultsubscriber.h"
#include "logic/dictation/transcriberwrapper.h"
#include "logic/search/isearch.h"
#include "logic/search/isearchoptionsmediator.h"

#include <QAction>
#include <QEvent>
#include <QFocusEvent>
#include <QLineEdit>
#include <QSettings>
#include <QTimer>
#include <QWidget>

#include <memory>
#include <string>
#include <variant>

// The SearchLineEdit is the main search bar

class SearchLineEdit : public QLineEdit,
                       public ISearchLineEdit,
                       public ITranscriptionResultSubscriber
{
    Q_OBJECT

public:
    explicit SearchLineEdit(std::shared_ptr<ISearchOptionsMediator> mediator,
                            std::shared_ptr<ISearch> manager,
                            std::shared_ptr<SQLUserHistoryUtils> sqlHistoryUtils,
                            QWidget *parent = nullptr);
    ~SearchLineEdit();

    void changeEvent(QEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

    void transcriptionResult(
        std::variant<std::system_error, std::string> transcription) override;

    void updateParameters(SearchParameters parameters) override;
    void search() override;

private:
    void setupUI(void);
    void translateUI(void);
    void setStyle(bool use_dark);

    void checkClearVisibility(void);

    void startTranscription(void);
    void stopTranscription(void);

    void addSearchTermToHistory(SearchParameters parameters) const;

    std::shared_ptr<ISearchOptionsMediator> _mediator;
    std::shared_ptr<ISearch> _search;
    std::shared_ptr<SQLUserHistoryUtils> _sqlHistoryUtils;
    std::unique_ptr<QSettings> _settings;

    QAction *_searchLineEdit;
    QAction *_clearLineEdit;
    QAction *_microphone;
    QAction *_microphoneOff;
    QTimer *_timer;

    SearchParameters _parameters;

    std::unique_ptr<TranscriberWrapper> _wrapper;

    bool _paletteRecentlyChanged = false;

public slots:
    void searchTriggered(void);
};

#endif // SEARCHLINEEDIT_H

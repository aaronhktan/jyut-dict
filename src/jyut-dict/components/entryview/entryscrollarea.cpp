#include "entryscrollarea.h"

#include "logic/entry/entry.h"

#include <QScrollBar>

EntryScrollArea::EntryScrollArea(std::shared_ptr<SQLUserDataUtils> sqlUserUtils,
                                 std::shared_ptr<SQLDatabaseManager> manager,
                                 QWidget *parent)
    : QScrollArea(parent)
{
    _enableUIUpdateTimer = new QTimer{this};
    _updateUITimer = new QTimer{this};

    setFrameShape(QFrame::NoFrame);
    verticalScrollBar()->setFocusPolicy(Qt::StrongFocus);

    _scrollAreaWidget = new EntryScrollAreaWidget{sqlUserUtils, manager, this};

    setWidget(_scrollAreaWidget);
    setWidgetResizable(
        true); // IMPORTANT! This makes the scrolling widget resize correctly.
#ifdef Q_OS_LINUX
    setMinimumWidth(520);
#else
    setMinimumWidth(500);
#endif

    connect(this,
            &EntryScrollArea::stallSentenceUIUpdate,
            _scrollAreaWidget,
            &EntryScrollAreaWidget::stallUISentenceUpdate);

    connect(this,
            &EntryScrollArea::favouriteCurrentEntry,
            _scrollAreaWidget,
            &EntryScrollAreaWidget::favouriteCurrentEntryRequested);

    connect(this,
            &EntryScrollArea::shareCurrentEntry,
            _scrollAreaWidget,
            &EntryScrollAreaWidget::shareCurrentEntryRequested);

    connect(this,
            &EntryScrollArea::openCurrentEntryInNewWindow,
            _scrollAreaWidget,
            &EntryScrollAreaWidget::openInNewWindow);

    connect(this,
            &EntryScrollArea::magnifyCurrentEntry,
            _scrollAreaWidget,
            &EntryScrollAreaWidget::openMagnifyWindow);

    connect(this,
            &EntryScrollArea::viewAllSentences,
            _scrollAreaWidget,
            &EntryScrollAreaWidget::viewAllSentencesRequested);

    connect(this,
            &EntryScrollArea::searchEntriesBeginning,
            _scrollAreaWidget,
            &EntryScrollAreaWidget::searchEntriesBeginningRequested);

    connect(this,
            &EntryScrollArea::searchEntriesContaining,
            _scrollAreaWidget,
            &EntryScrollAreaWidget::searchEntriesContainingRequested);

    connect(this,
            &EntryScrollArea::searchEntriesEnding,
            _scrollAreaWidget,
            &EntryScrollAreaWidget::searchEntriesEndingRequested);

    connect(_scrollAreaWidget,
            &EntryScrollAreaWidget::searchQuery,
            this,
            &EntryScrollArea::searchQueryRequested);

    if (!parent) {
        setMinimumHeight(400);
    }
}

void EntryScrollArea::keyPressEvent(QKeyEvent *event)
{
    if (isWindow() && event->key() == Qt::Key_Escape) {
        close();
        return;
    } else {
        // Not sure why setting the focus policy of the scroll bar
        // doesn't work, but this is a workaround!
        switch (event->key()) {
        case Qt::Key_Down: {
            verticalScrollBar()->triggerAction(
                QAbstractSlider::SliderSingleStepAdd);
            break;
        }
        case Qt::Key_Up: {
            verticalScrollBar()->triggerAction(
                QAbstractSlider::SliderSingleStepSub);
            break;
        }
        case Qt::Key_PageDown: {
            verticalScrollBar()->triggerAction(
                QAbstractSlider::SliderPageStepAdd);
            break;
        }
        case Qt::Key_PageUp: {
            verticalScrollBar()->triggerAction(
                QAbstractSlider::SliderPageStepSub);
            break;
        }
        case Qt::Key_Home: {
            verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMinimum);
            break;
        }
        case Qt::Key_End: {
            verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);
            break;
        }
        }
        return;
    }
    QWidget::keyPressEvent(event);
}

void EntryScrollArea::setEntry(const Entry &entry)
{
    _updateUITimer->stop();
    disconnect(_updateUITimer, nullptr, nullptr, nullptr);

    _updateUITimer->setInterval(25);
    QObject::connect(_updateUITimer, &QTimer::timeout, this, [=]() {
        if (_enableUIUpdate) {
            _updateUITimer->stop();
            disconnect(_updateUITimer, nullptr, nullptr, nullptr);
            _scrollAreaWidget->setEntry(entry);
            _scrollAreaWidget->setVisible(false);
            int largerHeight = std::max(_scrollAreaWidget->sizeHint().height(),
                                        height());
            _scrollAreaWidget->resize(width()
                                          - (verticalScrollBar()->isVisible()
                                                 ? verticalScrollBar()->width()
                                                 : 0),
                                      largerHeight);
            _scrollAreaWidget->setVisible(true);
            // Scroll to top of scrollable area when new entry is set
            ensureVisible(0, 0);
        }
    });
    _updateUITimer->start();
}

void EntryScrollArea::resizeEvent(QResizeEvent *event)
{
    int largerHeight = std::max(_scrollAreaWidget->sizeHint().height(),
                                height());
    _scrollAreaWidget->resize(width()
                                  - (verticalScrollBar()->isVisible()
                                         ? verticalScrollBar()->width()
                                         : 0),
                              largerHeight);
    event->accept();
}

// This slot is intended to be called after the user inputs text in the search
// box. The call to setEntry(entry) is very CPU intensive due to
// the time required to lay out many widgets, and it also must occur on the
// main GUI thread.
//
// We don't want to freeze the UI while the user types. To do that, we disable
// the call to setEntry(entry) for 200ms. A pause of 200ms is good enough to
// assume that the user is done typing, so the expensive GUI operation can run.
//
// However, we don't want to stall the UI update every time a call to
// setEntry(entry) occurs! For example, we don't want to stall the UI update
// if the user is clicking on a result in the list view. This is why the
// stalling behaviour is implemented as a slot instead of in the setEntry(entry)
// method.
void EntryScrollArea::stallEntryUIUpdate(void)
{
    _enableUIUpdate = false;
    _enableUIUpdateTimer->stop();
    disconnect(_enableUIUpdateTimer, nullptr, nullptr, nullptr);
    _enableUIUpdateTimer->setInterval(200);
    _enableUIUpdateTimer->setSingleShot(true);
    QObject::connect(_enableUIUpdateTimer, &QTimer::timeout, this, [=]() {
        _enableUIUpdate = true;
    });
    _enableUIUpdateTimer->start();
}

void EntryScrollArea::updateStyleRequested(void)
{
    _scrollAreaWidget->updateStyleRequested();
    _scrollAreaWidget->resize(width()
                                  - (verticalScrollBar()->isVisible()
                                         ? verticalScrollBar()->width()
                                         : 0),
                              _scrollAreaWidget->sizeHint().height());
}

void EntryScrollArea::favouriteCurrentEntryRequested(void)
{
    emit favouriteCurrentEntry();
}

void EntryScrollArea::shareCurrentEntryRequested(void)
{
    emit shareCurrentEntry();
}

void EntryScrollArea::openCurrentEntryInNewWindowRequested(void)
{
    emit openCurrentEntryInNewWindow();
}

void EntryScrollArea::magnifyCurrentEntryRequested(void)
{
    emit magnifyCurrentEntry();
}

void EntryScrollArea::viewAllSentencesRequested(void)
{
    emit viewAllSentences();
}

void EntryScrollArea::searchEntriesBeginningRequested(void)
{
    emit searchEntriesBeginning();
}

void EntryScrollArea::searchEntriesContainingRequested(void)
{
    emit searchEntriesContaining();
}

void EntryScrollArea::searchEntriesEndingRequested(void)
{
    emit searchEntriesEnding();
}

void EntryScrollArea::searchQueryRequested(const QString &query,
                                           const SearchParameters &parameters)
{
    emit searchQuery(query, parameters);
}

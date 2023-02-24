#include "entryscrollareawidget.h"

#include "components/entryview/entryscrollarea.h"
#include "components/magnifywindow/magnifyscrollarea.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif

#include <QTimer>

EntryScrollAreaWidget::EntryScrollAreaWidget(
    std::shared_ptr<SQLUserDataUtils> sqlUserUtils,
    std::shared_ptr<SQLDatabaseManager> manager,
    QWidget *parent)
    : QWidget(parent)
    , _sqlUserUtils{sqlUserUtils}
    , _manager{manager}
{
    setObjectName("EntryScrollAreaWidget");

    // Entire Scroll Area
    _scrollAreaLayout = new QGridLayout{this};
    _scrollAreaLayout->setContentsMargins(11, 11, 11, 11);
    _scrollAreaLayout->setSpacing(0);

    _entryHeaderWidget = new EntryHeaderWidget{this};
    _entryHeaderWidget->setMaximumWidth(800);
    _entryActionWidget = new EntryActionWidget{sqlUserUtils, this};
    _entryActionWidget->setMaximumWidth(800);
    _entryContentWidget = new EntryContentWidget{manager, this};
    _entryContentWidget->setMaximumWidth(800);

    // Add all widgets to main layout
    // Spacer on left
    _scrollAreaLayout->addItem(new QSpacerItem{0, 0}, 0, 0, 1, 1);
    // Spacer on right
    _scrollAreaLayout->addItem(new QSpacerItem{0, 0}, 0, 2, 1, 1);
    // Spacer at bottom
    _scrollAreaLayout->addItem(new QSpacerItem{0,
                                               0,
                                               QSizePolicy::Minimum,
                                               QSizePolicy::MinimumExpanding},
                               3,
                               1,
                               1,
                               1);

    _scrollAreaLayout->addWidget(_entryHeaderWidget, 0, 1, 1, 1);
    _scrollAreaLayout->addWidget(_entryActionWidget, 1, 1, 1, 1);
    _scrollAreaLayout->addWidget(_entryContentWidget, 2, 1, 1, 1);

    setStyle(Utils::isDarkMode());

    connect(this,
            &EntryScrollAreaWidget::stallUISentenceUpdate,
            _entryContentWidget,
            &EntryContentWidget::stallSentenceUIUpdate);

    connect(this,
            &EntryScrollAreaWidget::favouriteCurrentEntry,
            _entryActionWidget,
            &EntryActionWidget::favouriteCurrentEntryRequested);

    connect(this,
            &EntryScrollAreaWidget::shareCurrentEntry,
            _entryActionWidget,
            &EntryActionWidget::shareCurrentEntryRequested);

    connect(this,
            &EntryScrollAreaWidget::viewAllSentences,
            _entryContentWidget,
            &EntryContentWidget::viewAllSentencesRequested);

    connect(_entryActionWidget,
            &EntryActionWidget::openInNewWindow,
            this,
            &EntryScrollAreaWidget::openInNewWindow);

    connect(_entryActionWidget,
            &EntryActionWidget::openMagnifyWindow,
            this,
            &EntryScrollAreaWidget::openMagnifyWindow);

    connect(_entryContentWidget,
            &EntryContentWidget::searchQuery,
            this,
            &EntryScrollAreaWidget::searchQueryRequested);
}

void EntryScrollAreaWidget::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, this, [=]() { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
    QWidget::changeEvent(event);
}

void EntryScrollAreaWidget::setEntry(const Entry &entry)
{
    _entry = entry;
    _entryIsValid = true;
    _entryHeaderWidget->setEntry(entry);
    _entryActionWidget->setEntry(entry);
    _entryContentWidget->setEntry(entry);
}

void EntryScrollAreaWidget::setStyle(bool use_dark)
{
    (void) (use_dark);
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet("QWidget#EntryScrollAreaWidget { "
                  "   background-color: palette(base); "
                  "} ");
}

void EntryScrollAreaWidget::updateStyleRequested(void)
{
    QEvent event{QEvent::PaletteChange};
    QCoreApplication::sendEvent(_entryHeaderWidget, &event);
    QCoreApplication::sendEvent(_entryActionWidget, &event);

    _entryContentWidget->updateStyleRequested();

    QList<MagnifyScrollArea *> areas = findChildren<MagnifyScrollArea *>();
    for (auto &area : areas) {
        area->updateStyleRequested();
    }
}

void EntryScrollAreaWidget::favouriteCurrentEntryRequested(void)
{
    emit favouriteCurrentEntry();
}

void EntryScrollAreaWidget::shareCurrentEntryRequested(void)
{
    emit shareCurrentEntry();
}

void EntryScrollAreaWidget::openInNewWindow(void)
{
    if (!_entryIsValid) {
        return;
    }

    EntryScrollArea *area = new EntryScrollArea{_sqlUserUtils,
                                                _manager,
                                                nullptr};
    area->setParent(this, Qt::Window);
    area->setAttribute(Qt::WA_DeleteOnClose);
    area->setEntry(_entry);
#ifndef Q_OS_MAC
    area->setWindowTitle(" ");
#endif
    emit area->stallSentenceUIUpdate();
    area->show();
}

void EntryScrollAreaWidget::openMagnifyWindow(void)
{
    if (!_entryIsValid) {
        return;
    }

    MagnifyScrollArea *area = new MagnifyScrollArea{nullptr};
    area->setParent(this, Qt::Window);
    area->setAttribute(Qt::WA_DeleteOnClose);
    area->setEntry(_entry);
#ifndef Q_OS_MAC
    area->setWindowTitle(" ");
#endif
    area->show();
}

void EntryScrollAreaWidget::viewAllSentencesRequested(void)
{
    emit viewAllSentences();
}

void EntryScrollAreaWidget::searchQueryRequested(QString query,
                                                 SearchParameters parameters)
{
    emit searchQuery(query, parameters);
}

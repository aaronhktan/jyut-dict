#include "entryscrollareawidget.h"

#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QTimer>

EntryScrollAreaWidget::EntryScrollAreaWidget(std::shared_ptr<SQLUserDataUtils> sqlUserUtils,
                                             std::shared_ptr<SQLDatabaseManager> manager,
                                             QWidget *parent)
    : QWidget(parent)
{
    setObjectName("DefinitionScrollAreaWidget");

    // Entire Scroll Area
    _scrollAreaLayout = new QVBoxLayout{this};
    _scrollAreaLayout->setSpacing(0);
    _scrollAreaLayout->setContentsMargins(11, 11, 11, 11);

    _entryHeaderWidget = new EntryHeaderWidget{this};
    _entryActionWidget = new EntryActionWidget{sqlUserUtils, this};
    _entryContentWidget = new EntryContentWidget{manager, this};

    // Add all widgets to main layout
    _scrollAreaLayout->addWidget(_entryHeaderWidget);
    _scrollAreaLayout->addWidget(_entryActionWidget);
    _scrollAreaLayout->addWidget(_entryContentWidget);
    _scrollAreaLayout->addStretch(1);

#if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    setStyle(Utils::isDarkMode());
#else
    setStyle(/* use_dark = */false);
#endif

    connect(this,
            &EntryScrollAreaWidget::stallUIUpdate,
            _entryContentWidget,
            &EntryContentWidget::stallUIUpdate);
}

void EntryScrollAreaWidget::changeEvent(QEvent *event)
{
#if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, [=]() { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
#endif
    QWidget::changeEvent(event);
}

void EntryScrollAreaWidget::setEntry(const Entry &entry)
{
    _entryHeaderWidget->setEntry(entry);
    _entryActionWidget->setEntry(entry);
    _entryContentWidget->setEntry(entry);
}

void EntryScrollAreaWidget::setStyle(bool use_dark)
{
    QColor backgroundColour = use_dark ? QColor{BACKGROUND_COLOUR_DARK_R,
                                                BACKGROUND_COLOUR_DARK_G,
                                                BACKGROUND_COLOUR_DARK_B}
                                       : QColor{BACKGROUND_COLOUR_LIGHT_R,
                                                BACKGROUND_COLOUR_LIGHT_G,
                                                BACKGROUND_COLOUR_LIGHT_B};
    QString styleSheet = "QWidget#DefinitionScrollAreaWidget { background-color: %1; }";
    setStyleSheet(styleSheet.arg(backgroundColour.name()));
    setAttribute(Qt::WA_StyledBackground);
}

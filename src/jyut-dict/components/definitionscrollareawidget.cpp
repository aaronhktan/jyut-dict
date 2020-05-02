#include "definitionscrollareawidget.h"

#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QTimer>

DefinitionScrollAreaWidget::DefinitionScrollAreaWidget(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("DefinitionScrollAreaWidget");

    // Entire Scroll Area
    _scrollAreaLayout = new QVBoxLayout{this};
    _scrollAreaLayout->setSpacing(25);
    _scrollAreaLayout->setContentsMargins(11, 11, 11, 11);

    _entryHeaderWidget = new EntryHeaderWidget{this};
    _definitionWidget = new DefinitionWidget{this};
    _loadingWidget = new LoadingWidget{this};
    _loadingWidget->setVisible(false);

    // Add all widgets to main layout
    _scrollAreaLayout->addWidget(_entryHeaderWidget);
    _scrollAreaLayout->addWidget(_definitionWidget);
    _scrollAreaLayout->addWidget(_loadingWidget);
    _scrollAreaLayout->setAlignment(_loadingWidget, Qt::AlignHCenter);
    _scrollAreaLayout->addStretch(1);

#ifdef Q_OS_MAC
    setStyle(Utils::isDarkMode());
#else
    setStyle(/* use_dark = */false);
#endif
}

void DefinitionScrollAreaWidget::changeEvent(QEvent *event)
{
#if defined(Q_OS_DARWIN)
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

void DefinitionScrollAreaWidget::setEntry(const Entry &entry)
{
    _entryHeaderWidget->setEntry(entry);
    _definitionWidget->setEntry(entry);

    _loadingWidget->setVisible(true);
}

void DefinitionScrollAreaWidget::setStyle(bool use_dark)
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

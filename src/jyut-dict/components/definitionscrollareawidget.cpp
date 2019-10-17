#include "definitionscrollareawidget.h"

#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#endif

#include <QTimer>

DefinitionScrollAreaWidget::DefinitionScrollAreaWidget(QWidget *parent)
    : QWidget(parent)
{
    // Entire Scroll Area
    _scrollAreaLayout = new QVBoxLayout{this};
    _scrollAreaLayout->setSpacing(25);
    _scrollAreaLayout->setContentsMargins(11, 11, 11, 11);

    _entryHeaderWidget = new EntryHeaderWidget{this};
    _definitionWidget = new DefinitionWidget{this};

    // Add all widgets to main layout
    _scrollAreaLayout->addWidget(_entryHeaderWidget);
    _scrollAreaLayout->addWidget(_definitionWidget);
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
        QTimer::singleShot(100, [=]() { _paletteRecentlyChanged = false; });

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
}

void DefinitionScrollAreaWidget::setStyle(bool use_dark)
{
    if (use_dark) {
        setStyleSheet("QWidget { background-color: #1E1E1E; }");
    } else {
        setStyleSheet("QWidget { background-color: #FFFFFF; }");
    }
}

#include "sentencescrollareawidget.h"

#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QTimer>

SentenceScrollAreaWidget::SentenceScrollAreaWidget(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("EntryScrollAreaWidget");

    // Entire Scroll Area
    _scrollAreaLayout = new QVBoxLayout{this};
    _scrollAreaLayout->setSpacing(0);
    _scrollAreaLayout->setContentsMargins(11, 11, 11, 11);

    _sentenceViewHeaderWidget = new SentenceViewHeaderWidget{this};
    _sentenceViewContentWidget = new SentenceViewContentWidget{this};

    // Add all widgets to main layout
    _scrollAreaLayout->addWidget(_sentenceViewHeaderWidget);
    _scrollAreaLayout->addWidget(_sentenceViewContentWidget);
    _scrollAreaLayout->addStretch(2);
    setStyle(Utils::isDarkMode());
}

void SentenceScrollAreaWidget::changeEvent(QEvent *event)
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

void SentenceScrollAreaWidget::setSourceSentence(const SourceSentence &sentence)
{
    _sentenceViewHeaderWidget->setSourceSentence(sentence);
    _sentenceViewContentWidget->setSourceSentence(sentence);
}

void SentenceScrollAreaWidget::setStyle(bool use_dark)
{
    QColor backgroundColour = use_dark ? QColor{BACKGROUND_COLOUR_DARK_R,
                                                BACKGROUND_COLOUR_DARK_G,
                                                BACKGROUND_COLOUR_DARK_B}
                                       : QColor{BACKGROUND_COLOUR_LIGHT_R,
                                                BACKGROUND_COLOUR_LIGHT_G,
                                                BACKGROUND_COLOUR_LIGHT_B};
    QString styleSheet = "QWidget#EntryScrollAreaWidget { background-color: %1; }";
    setStyleSheet(styleSheet.arg(backgroundColour.name()));
    setAttribute(Qt::WA_StyledBackground);
}

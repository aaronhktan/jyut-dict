#include "sentencescrollareawidget.h"

#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif

#include <QCoreApplication>
#include <QTimer>

SentenceScrollAreaWidget::SentenceScrollAreaWidget(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("SentenceScrollAreaWidget");

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
    _sentence = sentence;
    _sentenceIsValid = true;

    _sentenceViewHeaderWidget->setSourceSentence(_sentence);
    _sentenceViewContentWidget->setSourceSentence(_sentence);
}

void SentenceScrollAreaWidget::setStyle(bool use_dark)
{
    (void) (use_dark);
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet("QWidget#SentenceScrollAreaWidget { "
                  "   background-color: palette(base); "
                  "} ");
}

void SentenceScrollAreaWidget::updateStyleRequested(void)
{
    if (_sentenceIsValid) {
        _sentenceViewHeaderWidget->setSourceSentence(_sentence);
    }
    QEvent event{QEvent::PaletteChange};
    QCoreApplication::sendEvent(_sentenceViewHeaderWidget, &event);
    _sentenceViewContentWidget->updateStyleRequested();
}

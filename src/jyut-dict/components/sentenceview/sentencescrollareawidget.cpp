#include "sentencescrollareawidget.h"

#include "components/sentenceview/sentenceviewcontentwidget.h"
#include "components/sentenceview/sentenceviewheaderwidget.h"
#include "logic/settings/settingsutils.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif

#include <QCoreApplication>
#include <QEvent>
#include <QTimer>
#include <QVBoxLayout>

SentenceScrollAreaWidget::SentenceScrollAreaWidget(QWidget *parent)
    : QWidget{parent}
    , _settings{Settings::getSettings(this)}
    , _scrollAreaLayout{new QVBoxLayout{this}}
    , _sentenceViewHeaderWidget{new SentenceViewHeaderWidget{this}}
    , _sentenceViewContentWidget{new SentenceViewContentWidget{this}}
{
    setObjectName("SentenceScrollAreaWidget");
    setAttribute(Qt::WA_StyledBackground);

    _scrollAreaLayout->setSpacing(0);
    _scrollAreaLayout->setContentsMargins(11, 11, 11, 11);

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
        QTimer::singleShot(10, this, [&] { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
    QWidget::changeEvent(event);
}

void SentenceScrollAreaWidget::setSourceSentence(const SourceSentence &sentence)
{
    _sentence = sentence;

    _sentenceViewHeaderWidget->setSourceSentence(_sentence.value());
    _sentenceViewContentWidget->setSourceSentence(_sentence.value());
}

void SentenceScrollAreaWidget::setStyle([[maybe_unused]] bool use_dark)
{
    setStyleSheet("QWidget#SentenceScrollAreaWidget { "
                  "   background-color: palette(base); "
                  "} ");
}

void SentenceScrollAreaWidget::updateStyleRequested(void)
{
    if (_sentence.has_value()) {
        CantoneseOptions cantoneseOptions
            = _settings
                  ->value("Entry/cantonesePronunciationOptions",
                          QVariant::fromValue(CantoneseOptions::RAW_JYUTPING))
                  .value<CantoneseOptions>();
        MandarinOptions mandarinOptions
            = _settings
                  ->value("Entry/mandarinPronunciationOptions",
                          QVariant::fromValue(MandarinOptions::PRETTY_PINYIN))
                  .value<MandarinOptions>();
        _sentence.value().generatePhonetic(cantoneseOptions, mandarinOptions);
        _sentenceViewHeaderWidget->setSourceSentence(_sentence.value());
    }
    QEvent event{QEvent::PaletteChange};
    QCoreApplication::sendEvent(_sentenceViewHeaderWidget, &event);
    _sentenceViewContentWidget->updateStyleRequested();
}

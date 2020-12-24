#include "sentencecardwidget.h"

#include "logic/strings/strings.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QCoreApplication>
#include <QTimer>

SentenceCardWidget::SentenceCardWidget(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("SentenceCardWidget");
    setAttribute(Qt::WA_StyledBackground, true);

    _sentenceCardLayout = new QVBoxLayout{this};
    _sentenceCardLayout->setContentsMargins(0, 0, 0, 0);
    _sentenceCardLayout->setSpacing(11);

    _sentenceHeaderWidget = new SentenceHeaderWidget{this};
    _sentenceContentWidget = new SentenceContentWidget{this};

    _sentenceCardLayout->addWidget(_sentenceHeaderWidget);
    _sentenceCardLayout->addWidget(_sentenceContentWidget);

#if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    setStyle(Utils::isDarkMode());
#else
    setStyle(/* use_dark = */ false);
#endif
}

void SentenceCardWidget::changeEvent(QEvent *event)
{
#if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, [&]() { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
#endif
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QWidget::changeEvent(event);
}

void SentenceCardWidget::displaySentences(const std::vector<SourceSentence> &sentences)
{
    if (sentences.empty()) {
        return;
    }

    _source = sentences.at(0).getSentenceSets().at(0).getSourceShortString();
    _sentenceHeaderWidget->setCardTitle(
        QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                    Strings::SENTENCES_ALL_CAPS)
            .toStdString()
        + " (" + _source + ")");
    _sentenceContentWidget->setSourceSentenceVector(sentences);

#if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    setStyle(Utils::isDarkMode());
#else
    setStyle(/* use_dark = */false);
#endif
}

void SentenceCardWidget::displaySentences(const SentenceSet &set)
{
    if (set.getSentences().empty()) {
        return;
    }

    _source = set.getSourceShortString();
    _sentenceHeaderWidget->setCardTitle(
        QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                    Strings::SENTENCES_ALL_CAPS)
            .toStdString()
        + " (" + _source + ")");
    _sentenceContentWidget->setSentenceSet(set);

#if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    setStyle(Utils::isDarkMode());
#else
    setStyle(/* use_dark = */false);
#endif
}

void SentenceCardWidget::translateUI()
{
    _sentenceHeaderWidget->setCardTitle(
        QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                    Strings::SENTENCES_ALL_CAPS)
            .toStdString()
        + " (" + _source + ")");
}

void SentenceCardWidget::setStyle(bool use_dark)
{
    QString styleSheet = "QWidget#SentenceCardWidget { "
                         " background-color: %1; "
                         " border-radius: 10px; "
                         "}";
    QColor backgroundColour = use_dark ? QColor{CONTENT_BACKGROUND_COLOUR_DARK_R,
                                                CONTENT_BACKGROUND_COLOUR_DARK_G,
                                                CONTENT_BACKGROUND_COLOUR_DARK_B}
                                       : QColor{CONTENT_BACKGROUND_COLOUR_LIGHT_R,
                                                CONTENT_BACKGROUND_COLOUR_LIGHT_G,
                                                CONTENT_BACKGROUND_COLOUR_LIGHT_B};
    setStyleSheet(styleSheet.arg(backgroundColour.name()));
}


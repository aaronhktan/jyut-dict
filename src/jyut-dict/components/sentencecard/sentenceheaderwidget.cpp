#include "sentenceheaderwidget.h"

#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QTimer>

SentenceHeaderWidget::SentenceHeaderWidget(QWidget *parent) : QWidget(parent)
{
    setObjectName("DefinitionHeaderWidget");

    _layout = new QVBoxLayout{this};
    _layout->setContentsMargins(10, 10, 10, 10);
    _layout->setSpacing(10);

    _titleLabel = new QLabel{this};
    _titleLabel->setObjectName("DefinitionHeaderWidgetTitleLabel");

    _layout->addWidget(_titleLabel);

#if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    setStyle(Utils::isDarkMode());
#else
    setStyle(/* use_dark = */false);
#endif
}

SentenceHeaderWidget::SentenceHeaderWidget(std::string title, QWidget *parent)
    : SentenceHeaderWidget(parent)
{
    setCardTitle(title);
}

void SentenceHeaderWidget::changeEvent(QEvent *event)
{
#if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, this, [=]() { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
#endif
    QWidget::changeEvent(event);
}

void SentenceHeaderWidget::setCardTitle(std::string title)
{
    _titleLabel->setText(title.c_str());
    _titleLabel->setFixedHeight(_titleLabel->fontMetrics().boundingRect(title.c_str()).height());
}

void SentenceHeaderWidget::setStyle(bool use_dark)
{
    // Style the main background
    QString widgetStyleSheet = "QWidget#DefinitionHeaderWidget { "
                               " background-color: %1; "
                               " border-top-left-radius: 10px; "
                               " border-top-right-radius: 10px; "
                               " border-bottom-left-radius: 0px; "
                               " border-bottom-right-radius: 0px; "
                               "}";
    QColor backgroundColour
        = use_dark ? QColor{HEADER_BACKGROUND_COLOUR_DARK_R,
                            HEADER_BACKGROUND_COLOUR_DARK_G,
                            HEADER_BACKGROUND_COLOUR_DARK_B}
                   : QColor{HEADER_BACKGROUND_COLOUR_LIGHT_R,
                            HEADER_BACKGROUND_COLOUR_LIGHT_G,
                            HEADER_BACKGROUND_COLOUR_LIGHT_B};
    setStyleSheet(widgetStyleSheet.arg(backgroundColour.name()));

    // Style the label text
    QString textStyleSheet = "QLabel#DefinitionHeaderWidgetTitleLabel { color: %1; }";
    QColor textColour = use_dark ? QColor{LABEL_TEXT_COLOUR_DARK_R,
                                          LABEL_TEXT_COLOUR_DARK_G,
                                          LABEL_TEXT_COLOUR_DARK_B}
                                 : QColor{LABEL_TEXT_COLOUR_LIGHT_R,
                                          LABEL_TEXT_COLOUR_LIGHT_R,
                                          LABEL_TEXT_COLOUR_LIGHT_R};
    _titleLabel->setStyleSheet(textStyleSheet.arg(textColour.name()));
}

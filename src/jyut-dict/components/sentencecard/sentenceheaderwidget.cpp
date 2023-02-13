#include "sentenceheaderwidget.h"

#include "logic/settings/settings.h"
#include "logic/settings/settingsutils.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QTimer>

SentenceHeaderWidget::SentenceHeaderWidget(QWidget *parent) : QWidget(parent)
{
    _settings = Settings::getSettings(this);

    setObjectName("SentenceHeaderWidget");

    _layout = new QVBoxLayout{this};
    _layout->setContentsMargins(10, 10, 10, 10);
    _layout->setSpacing(10);

    _titleLabel = new QLabel{this};
    _titleLabel->setObjectName("SentenceHeaderWidgetTitleLabel");

    _layout->addWidget(_titleLabel);

    setStyle(Utils::isDarkMode());
}

SentenceHeaderWidget::SentenceHeaderWidget(std::string title, QWidget *parent)
    : SentenceHeaderWidget(parent)
{
    setCardTitle(title);
}

void SentenceHeaderWidget::changeEvent(QEvent *event)
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

void SentenceHeaderWidget::setCardTitle(const std::string &title)
{
    _titleLabel->setText(title.c_str());
    _titleLabel->setFixedHeight(_titleLabel->fontMetrics().boundingRect(title.c_str()).height());
}

void SentenceHeaderWidget::setStyle(bool use_dark)
{
    // Style the main background
    QString widgetStyleSheet = "QWidget#SentenceHeaderWidget { "
                               " background-color: %1; "
                               " border-top-left-radius: 10px; "
                               " border-top-right-radius: 10px; "
                               " border-bottom-left-radius: 0px; "
                               " border-bottom-right-radius: 0px; "
                               "}";
    QColor backgroundColour = use_dark
                                  ? QColor{HEADER_BACKGROUND_COLOUR_DARK_R,
                                           HEADER_BACKGROUND_COLOUR_DARK_G,
                                           HEADER_BACKGROUND_COLOUR_DARK_B}
                                  : QColor{CONTENT_BACKGROUND_COLOUR_LIGHT_R,
                                           CONTENT_BACKGROUND_COLOUR_LIGHT_G,
                                           CONTENT_BACKGROUND_COLOUR_LIGHT_B};
    setStyleSheet(widgetStyleSheet.arg(backgroundColour.name()));

    // Style the label text
    int interfaceSize = static_cast<int>(
        _settings
            ->value("Interface/size",
                    QVariant::fromValue(Settings::InterfaceSize::NORMAL))
            .value<Settings::InterfaceSize>());
    int bodyFontSize = Settings::bodyFontSize.at(
        static_cast<unsigned long>(interfaceSize - 1));

    QString textStyleSheet = "QLabel#SentenceHeaderWidgetTitleLabel { "
                             "   color: %1; "
                             "   font-size: %2px; "
                             "}";
    QColor textColour = use_dark ? QColor{LABEL_TEXT_COLOUR_DARK_R,
                                          LABEL_TEXT_COLOUR_DARK_G,
                                          LABEL_TEXT_COLOUR_DARK_B}
                                 : QColor{LABEL_TEXT_COLOUR_LIGHT_R,
                                          LABEL_TEXT_COLOUR_LIGHT_R,
                                          LABEL_TEXT_COLOUR_LIGHT_R};
    _titleLabel->setStyleSheet(
        textStyleSheet.arg(textColour.name()).arg(bodyFontSize));
}

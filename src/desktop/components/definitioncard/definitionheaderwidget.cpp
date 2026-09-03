#include "definitionheaderwidget.h"

#include "logic/settings/settings.h"
#include "logic/settings/settingsutils.h"
#include "logic/strings/strings.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined(Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QCoreApplication>
#include <QEvent>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>

DefinitionHeaderWidget::DefinitionHeaderWidget(QWidget *parent)
    : QWidget{parent}
    , _settings{Settings::getSettings(this)}
    , _layout{new QVBoxLayout{this}}
    , _titleLabel{new QLabel{this}}
{
    setObjectName("DefinitionHeaderWidget");

    _layout->setContentsMargins(10, 10, 10, 10);
    _layout->setSpacing(10);

    _titleLabel->setObjectName("DefinitionHeaderWidgetTitleLabel");
    _titleLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    _layout->addWidget(_titleLabel);

    setStyle(Utils::isDarkMode());
}

void DefinitionHeaderWidget::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, this, [this] { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QWidget::changeEvent(event);
}

void DefinitionHeaderWidget::setSource(const std::string &source)
{
    setStyle(Utils::isDarkMode());
    _source = source;
    _titleLabel->setText(
        QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                    Strings::DEFINITIONS_ALL_CAPS)
        % " (" % QString::fromStdString(source) % ")");
    _titleLabel->setFixedHeight(
        _titleLabel->fontMetrics().boundingRect(_titleLabel->text()).height());
    resize(minimumSizeHint());
}

void DefinitionHeaderWidget::translateUI()
{
    setSource(_source);
}

void DefinitionHeaderWidget::setStyle(bool use_dark)
{
    // Style the main background
    QString widgetStyleSheet = "QWidget#DefinitionHeaderWidget { "
                               " background: %1; "
                               " border-top-left-radius: 10px; "
                               " border-top-right-radius: 10px; "
                               " border-bottom-left-radius: 0px; "
                               " border-bottom-right-radius: 0px; "
                               "}";
    QColor backgroundColour
        = use_dark ? QColor{Utils::HEADER_BACKGROUND_COLOUR_DARK_R,
                            Utils::HEADER_BACKGROUND_COLOUR_DARK_G,
                            Utils::HEADER_BACKGROUND_COLOUR_DARK_B}
                   : QColor{Utils::CONTENT_BACKGROUND_COLOUR_LIGHT_R,
                            Utils::CONTENT_BACKGROUND_COLOUR_LIGHT_G,
                            Utils::CONTENT_BACKGROUND_COLOUR_LIGHT_B};
    setStyleSheet(widgetStyleSheet.arg(backgroundColour.name()));

    // Style the label text
    int interfaceSize = static_cast<int>(
        _settings
            ->value("Interface/size",
                    QVariant::fromValue(Settings::InterfaceSize::NORMAL))
            .value<Settings::InterfaceSize>());
    int bodyFontSize = Settings::bodyFontSize.at(
        static_cast<unsigned long>(interfaceSize - 1));

    QString textStyleSheet = "QLabel#DefinitionHeaderWidgetTitleLabel { "
                             "   color: %1; "
                             "   font-size: %2px; "
                             "}";
    QColor textColour = use_dark ? QColor{Utils::LABEL_TEXT_COLOUR_DARK_R,
                                          Utils::LABEL_TEXT_COLOUR_DARK_G,
                                          Utils::LABEL_TEXT_COLOUR_DARK_B}
                                 : QColor{Utils::LABEL_TEXT_COLOUR_LIGHT_R,
                                          Utils::LABEL_TEXT_COLOUR_LIGHT_R,
                                          Utils::LABEL_TEXT_COLOUR_LIGHT_R};
    _titleLabel->setStyleSheet(
        textStyleSheet.arg(textColour.name()).arg(bodyFontSize));
    _titleLabel->setFixedHeight(
        _titleLabel->fontMetrics().boundingRect(_titleLabel->text()).height());
    resize(minimumSizeHint());
}

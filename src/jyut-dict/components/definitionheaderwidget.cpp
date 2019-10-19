#include "definitionheaderwidget.h"

#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#endif
#ifdef Q_OS_WIN
#include "logic/utils/utils_windows.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QTimer>

DefinitionHeaderWidget::DefinitionHeaderWidget(QWidget *parent)
    : QWidget(parent)
{
    _layout = new QVBoxLayout{this};
    _layout->setContentsMargins(10, 10, 10, 10);
    _layout->setSpacing(10);

    _titleLabel = new QLabel{this};

    _layout->addWidget(_titleLabel);

#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
    setStyle(Utils::isDarkMode());
#else
    setStyle(/* use_dark = */false);
#endif
}

DefinitionHeaderWidget::DefinitionHeaderWidget(std::string title, QWidget *parent)
    : DefinitionHeaderWidget(parent)
{
    setSectionTitle(title);
}

void DefinitionHeaderWidget::changeEvent(QEvent *event)
{
#ifdef Q_OS_MAC
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

DefinitionHeaderWidget::~DefinitionHeaderWidget()
{

}

void DefinitionHeaderWidget::setSectionTitle(std::string title)
{
    _titleLabel->setText(title.c_str());
    _titleLabel->setFixedHeight(_titleLabel->fontMetrics().boundingRect(title.c_str()).height());
}

void DefinitionHeaderWidget::setStyle(bool use_dark)
{
    // Style the main background
    QString widgetStyleSheet = "QWidget { "
                               " background-color: %1; "
                               " border-top-left-radius: 10px; "
                               " border-top-right-radius: 10px; "
                               " border-bottom-left-radius: 0px; "
                               " border-bottom-right-radius: 0px; "
                               "}";
    QColor backgroundColour = use_dark ? Utils::HEADER_BACKGROUND_COLOUR_DARK
                                       : Utils::HEADER_BACKGROUND_COLOUR_LIGHT;
    setStyleSheet(widgetStyleSheet.arg(backgroundColour.name()));

    // Style the label text
    QString textStyleSheet = "QLabel { color: %1; }";
    QColor textColour = use_dark ? Utils::LABEL_TEXT_COLOUR_DARK
                                 : Utils::LABEL_TEXT_COLOUR_LIGHT;
    _titleLabel->setStyleSheet(textStyleSheet.arg(textColour.name()));
}

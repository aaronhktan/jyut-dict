#include "definitionheaderwidget.h"

#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#endif

#include <QTimer>

DefinitionHeaderWidget::DefinitionHeaderWidget(QWidget *parent)
    : QWidget(parent)
{
    _layout = new QVBoxLayout{this};
    _layout->setContentsMargins(10, 10, 10, 10);
    _layout->setSpacing(10);

    _titleLabel = new QLabel{this};

    _layout->addWidget(_titleLabel);

#if defined(Q_OS_MAC)
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
    std::string formattedTitle = formatTitle(title);
    _titleLabel->setText(formattedTitle.c_str());
    _titleLabel->setFixedHeight(_titleLabel->fontMetrics().boundingRect(title.c_str()).height());
}

std::string DefinitionHeaderWidget::formatTitle(std::string title)
{
    return "<font color=#6F6F6F>" + title + "</font>";
}

void DefinitionHeaderWidget::setStyle(bool use_dark)
{
    QColor backgroundColour;
    QString styleSheet = "QWidget { "
                         " background-color: %1; "
                         " border-top-left-radius: 10px; "
                         " border-top-right-radius: 10px; "
                         " border-bottom-left-radius: 0px; "
                         " border-bottom-right-radius: 0px; "
                         "}";
    if (use_dark) {
        backgroundColour = QColor{60, 60, 60};
    } else {
        backgroundColour = QColor{235, 235, 235};
    }
    setStyleSheet(styleSheet.arg(backgroundColour.name()));
}

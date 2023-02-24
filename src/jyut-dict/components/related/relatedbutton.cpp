#include "relatedbutton.h"

#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined(Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QTimer>

RelatedButton::RelatedButton(RelatedType type, QWidget *parent)
    : QWidget{parent}
    , _buttonType{type}
{
    setObjectName("RelatedButton");
    setAttribute(Qt::WA_StyledBackground);

    setupUI();
    setStyle(Utils::isDarkMode());
}

void RelatedButton::changeEvent(QEvent *event)
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

void RelatedButton::setEntry(const Entry &entry)
{
    _content->setEntry(entry);
}

void RelatedButton::setupUI()
{
    _relatedButtonLayout = new QVBoxLayout{this};
    _relatedButtonLayout->setContentsMargins(0, 0, 0, 0);
    _relatedButtonLayout->setSpacing(0);

    _header = new RelatedButtonHeaderWidget{this};
    _content = new RelatedButtonContentWidget{_buttonType, this};

    _relatedButtonLayout->addWidget(_header);
    _relatedButtonLayout->addWidget(_content);

    connect(_content,
            &RelatedButtonContentWidget::searchQuery,
            this,
            &RelatedButton::searchQueryRequested);

    setMinimumHeight(150);
}

void RelatedButton::setStyle(bool use_dark)
{
    QColor borderColour = use_dark ? QColor{CONTENT_BACKGROUND_COLOUR_DARK_R,
                                            CONTENT_BACKGROUND_COLOUR_DARK_G,
                                            CONTENT_BACKGROUND_COLOUR_DARK_B}
                                   : QColor{CONTENT_BACKGROUND_COLOUR_LIGHT_R,
                                            CONTENT_BACKGROUND_COLOUR_LIGHT_G,
                                            CONTENT_BACKGROUND_COLOUR_LIGHT_B};
    QString widgetStyleSheet;
    if (use_dark) {
        widgetStyleSheet = "QWidget#RelatedButton { "
                           " background: %1; "
                           " border-radius: 10px; "
                           "}";
    } else {
        widgetStyleSheet = "QWidget#RelatedButton { "
                           " border: 1px solid %1; "
                           " border-radius: 10px; "
                           "}";
    }
    setStyleSheet(widgetStyleSheet.arg(borderColour.name()));
}

void RelatedButton::searchQueryRequested(QString query,
                                         SearchParameters parameters)
{
    emit searchQuery(query, parameters);
}

#include "definitioncardwidget.h"

#include "components/definitioncard/definitioncontentwidget.h"
#include "components/definitioncard/definitionheaderwidget.h"
#include "logic/entry/definitionsset.h"
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
#include <QStyle>
#include <QTimer>
#include <QVBoxLayout>

DefinitionCardWidget::DefinitionCardWidget(QWidget *parent)
    : QWidget{parent}
    , _definitionAreaLayout{new QVBoxLayout{this}}
    , _definitionHeaderWidget{new DefinitionHeaderWidget{this}}
    , _definitionContentWidget{new DefinitionContentWidget{this}}
{
    setObjectName("DefinitionCardWidget");

    _definitionAreaLayout->setContentsMargins(0, 0, 0, 0);
    _definitionAreaLayout->setSpacing(11);

    _definitionAreaLayout->addWidget(_definitionHeaderWidget);
    _definitionAreaLayout->addWidget(_definitionContentWidget);

    setStyle(Utils::isDarkMode());
}

void DefinitionCardWidget::changeEvent(QEvent *event)
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

void DefinitionCardWidget::setDefinitions(const DefinitionsSet &definitionsSet)
{
    _definitionHeaderWidget->setSource(definitionsSet.getSourceShortString());
    _definitionContentWidget->setDefinitions(definitionsSet.getDefinitions());
}

void DefinitionCardWidget::setStyle(bool use_dark)
{
    QString styleSheet;
    if (use_dark) {
        styleSheet = "QWidget#DefinitionCardWidget { "
                     " background: %1; "
                     " border-radius: 10px; "
                     "}";
    } else {
        styleSheet = "QWidget#DefinitionCardWidget { "
                     " border: 1px solid %1; "
                     " border-radius: 10px; "
                     "}";
    }
    QColor backgroundColour
        = use_dark ? QColor{Utils::CONTENT_BACKGROUND_COLOUR_DARK_R,
                            Utils::CONTENT_BACKGROUND_COLOUR_DARK_G,
                            Utils::CONTENT_BACKGROUND_COLOUR_DARK_B}
                   : QColor{Utils::CONTENT_BACKGROUND_COLOUR_LIGHT_R,
                            Utils::CONTENT_BACKGROUND_COLOUR_LIGHT_G,
                            Utils::CONTENT_BACKGROUND_COLOUR_LIGHT_B};
    setStyleSheet(styleSheet.arg(backgroundColour.name()));
}

void DefinitionCardWidget::updateStyleRequested(void)
{
    QEvent event{QEvent::PaletteChange};
    QCoreApplication::sendEvent(_definitionHeaderWidget, &event);
    QCoreApplication::sendEvent(_definitionContentWidget, &event);
}

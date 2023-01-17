#include "definitioncardwidget.h"

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
#include <QStyle>
#include <QTimer>

DefinitionCardWidget::DefinitionCardWidget(QWidget *parent) : QWidget(parent)
{
    setObjectName("DefinitionCardWidget");

    _definitionAreaLayout = new QVBoxLayout{this};
    _definitionAreaLayout->setContentsMargins(0, 0, 0, 0);
    _definitionAreaLayout->setSpacing(11);

    _definitionHeaderWidget = new DefinitionHeaderWidget{this};
    _definitionContentWidget = new DefinitionContentWidget{this};

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
        QTimer::singleShot(10, this, [&]() { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QWidget::changeEvent(event);
}

void DefinitionCardWidget::setEntry(const DefinitionsSet &definitionsSet)
{
    _source = definitionsSet.getSourceShortString();
    _definitionHeaderWidget->setSectionTitle(
        QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                    Strings::DEFINITIONS_ALL_CAPS)
            .toStdString()
        + " (" + _source + ")");

    _definitionContentWidget->setEntry(definitionsSet.getDefinitions());
}

void DefinitionCardWidget::translateUI()
{
    _definitionHeaderWidget->setSectionTitle(
        QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                    Strings::DEFINITIONS_ALL_CAPS)
            .toStdString()
        + " (" + _source + ")");
}

void DefinitionCardWidget::setStyle(bool use_dark)
{
    QString styleSheet = "QWidget#DefinitionCardWidget { "
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

void DefinitionCardWidget::updateStyleRequested(void)
{
    QEvent event{QEvent::PaletteChange};
    QCoreApplication::sendEvent(_definitionHeaderWidget, &event);
    QCoreApplication::sendEvent(_definitionContentWidget, &event);
}

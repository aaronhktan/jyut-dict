#include "definitioncardwidget.h"

#include "logic/strings/strings.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QCoreApplication>
#include <QStyle>
#include <QTimer>

DefinitionCardWidget::DefinitionCardWidget(QWidget *parent) : QWidget(parent)
{
    setObjectName("DefinitionSectionWidget");

    _definitionAreaLayout = new QVBoxLayout{this};
    _definitionAreaLayout->setContentsMargins(0, 0, 0, 0);
    _definitionAreaLayout->setSpacing(11);

    _definitionHeaderWidget = new DefinitionHeaderWidget{this};
    _definitionWidget = new DefinitionContentWidget{this};

    _definitionAreaLayout->addWidget(_definitionHeaderWidget);
    _definitionAreaLayout->addWidget(_definitionWidget);

#ifdef Q_OS_MAC
    setStyle(Utils::isDarkMode());
#else
    setStyle(/* use_dark = */false);
#endif
}

DefinitionCardWidget::~DefinitionCardWidget()
{
}

void DefinitionCardWidget::changeEvent(QEvent *event)
{
#ifdef Q_OS_MAC
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

void DefinitionCardWidget::setEntry(const DefinitionsSet &definitionsSet)
{
    _source = definitionsSet.getSourceShortString();
    _definitionHeaderWidget->setSectionTitle(
        QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                    Strings::DEFINITIONS_ALL_CAPS)
            .toStdString()
        + " (" + _source + ")");

    _definitionWidget->setEntry(definitionsSet.getDefinitions());
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
    QString styleSheet = "QWidget#DefinitionSectionWidget { "
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

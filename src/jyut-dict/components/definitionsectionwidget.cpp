#include "definitionsectionwidget.h"

#ifdef Q_OS_MAC
#include <logic/utils/utils_mac.h>
#endif

#include <QStyle>
#include <QTimer>

DefinitionSectionWidget::DefinitionSectionWidget(QWidget *parent) : QWidget(parent)
{
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

DefinitionSectionWidget::~DefinitionSectionWidget()
{

}

void DefinitionSectionWidget::changeEvent(QEvent *event)
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

void DefinitionSectionWidget::setEntry(const DefinitionsSet &definitionsSet)
{
    std::string source = definitionsSet.getSourceShortString();
    _definitionHeaderWidget->setSectionTitle("DEFINITIONS (" + source + ")");

    _definitionWidget->setEntry(definitionsSet.getDefinitions());
}

void DefinitionSectionWidget::setStyle(bool use_dark)
{
    QColor backgroundColour;
    QString styleSheet = "QWidget { "
                         " background-color: %1; "
                         " border-radius: 10px; "
                         "}";
    if (use_dark) {
        backgroundColour = QColor{50, 50, 50};
    } else {
#ifdef Q_OS_MAC
        backgroundColour = QColor{245, 245, 245};
#else
        backgroundColour = QColor{253, 253, 253};
#endif
    }
    setStyleSheet(styleSheet.arg(backgroundColour.name()));
}

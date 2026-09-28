#include "examplecardwidget.h"

#include "components/examplecard/examplecontentwidget.h"
#include "components/examplecard/exampleheaderwidget.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QCoreApplication>
#include <QEvent>
#include <QTimer>
#include <QVBoxLayout>

ExampleCardWidget::ExampleCardWidget(QWidget *parent)
    : QWidget{parent}
    , _exampleCardLayout{new QVBoxLayout{this}}
    , _exampleHeaderWidget{new ExampleHeaderWidget{this}}
    , _exampleContentWidget{new ExampleContentWidget{this}}
{
    setObjectName("ExampleCardWidget");
    setAttribute(Qt::WA_StyledBackground, true);

    _exampleCardLayout->setContentsMargins(0, 0, 0, 0);
    _exampleCardLayout->setSpacing(11);

    _exampleCardLayout->addWidget(_exampleHeaderWidget);
    _exampleCardLayout->addWidget(_exampleContentWidget);

    setStyle(Utils::isDarkMode());
}

void ExampleCardWidget::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, this, [this] { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
    QWidget::changeEvent(event);
}

void ExampleCardWidget::displayExamples(std::span<const Example> examples)
{
    if (examples.empty()) {
        return;
    }

    _examples = std::vector<Example>();
    _examples.value().assign(examples.begin(), examples.end());

    _exampleHeaderWidget->setSource(
        examples[0].getTranslationSets()[0].getSourceShortString());
    _exampleContentWidget->setExampleVector(_examples.value());

    setStyle(Utils::isDarkMode());
}

void ExampleCardWidget::displayTranslations(const TranslationSet &set)
{
    if (set.getTranslations().empty()) {
        return;
    }

    _exampleHeaderWidget->setSource(set.getSourceShortString());
    _exampleContentWidget->setTranslationSet(set);

    setStyle(Utils::isDarkMode());
}

void ExampleCardWidget::setStyle(bool use_dark)
{
    QString styleSheet;
    if (use_dark) {
        styleSheet = "QWidget#ExampleCardWidget { "
                     " background: %1; "
                     " border-radius: 10px; "
                     "}";
    } else {
        styleSheet = "QWidget#ExampleCardWidget { "
                     " border: 1px solid %1; "
                     " border-radius: 10px; "
                     "}";
    }
    const QColor backgroundColour
        = use_dark ? QColor{Utils::CONTENT_BACKGROUND_COLOUR_DARK_R,
                            Utils::CONTENT_BACKGROUND_COLOUR_DARK_G,
                            Utils::CONTENT_BACKGROUND_COLOUR_DARK_B}
                   : QColor{Utils::CONTENT_BACKGROUND_COLOUR_LIGHT_R,
                            Utils::CONTENT_BACKGROUND_COLOUR_LIGHT_G,
                            Utils::CONTENT_BACKGROUND_COLOUR_LIGHT_B};
    setStyleSheet(styleSheet.arg(backgroundColour.name()));
}

void ExampleCardWidget::updateStyleRequested(void)
{
    if (_examples.has_value()) {
        _exampleContentWidget->setExampleVector(_examples.value());
    }

    QEvent event{QEvent::PaletteChange};
    QCoreApplication::sendEvent(_exampleHeaderWidget, &event);
    QCoreApplication::sendEvent(_exampleContentWidget, &event);
}

#include "examplescrollareawidget.h"

#include "components/exampleview/exampleviewcontentwidget.h"
#include "components/exampleview/exampleviewheaderwidget.h"
#include "logic/settings/settingsutils.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif

#include <QCoreApplication>
#include <QEvent>
#include <QTimer>
#include <QVBoxLayout>

ExampleScrollAreaWidget::ExampleScrollAreaWidget(QWidget *parent)
    : QWidget{parent}
    , _settings{Settings::getSettings(this)}
    , _scrollAreaLayout{new QVBoxLayout{this}}
    , _exampleViewHeaderWidget{new ExampleViewHeaderWidget{this}}
    , _exampleViewContentWidget{new ExampleViewContentWidget{this}}
{
    setObjectName("ExampleScrollAreaWidget");
    setAttribute(Qt::WA_StyledBackground);

    _scrollAreaLayout->setSpacing(0);
    _scrollAreaLayout->setContentsMargins(11, 11, 11, 11);

    _scrollAreaLayout->addWidget(_exampleViewHeaderWidget);
    _scrollAreaLayout->addWidget(_exampleViewContentWidget);
    _scrollAreaLayout->addStretch(2);
    setStyle(Utils::isDarkMode());
}

void ExampleScrollAreaWidget::changeEvent(QEvent *event)
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

void ExampleScrollAreaWidget::setExample(const Example &example)
{
    _example = example;

    _exampleViewHeaderWidget->setExample(_example.value());
    _exampleViewContentWidget->setExample(_example.value());
}

void ExampleScrollAreaWidget::setStyle([[maybe_unused]] bool use_dark)
{
    setStyleSheet("QWidget#ExampleScrollAreaWidget { "
                  "   background-color: palette(base); "
                  "} ");
}

void ExampleScrollAreaWidget::updateStyleRequested(void)
{
    if (_example.has_value()) {
        CantoneseOptions cantoneseOptions
            = _settings
                  ->value("Entry/cantonesePronunciationOptions",
                          QVariant::fromValue(CantoneseOptions::RAW_JYUTPING))
                  .value<CantoneseOptions>();
        MandarinOptions mandarinOptions
            = _settings
                  ->value("Entry/mandarinPronunciationOptions",
                          QVariant::fromValue(MandarinOptions::PRETTY_PINYIN))
                  .value<MandarinOptions>();
        _example.value().generatePhonetic(cantoneseOptions, mandarinOptions);
        _exampleViewHeaderWidget->setExample(_example.value());
    }
    QEvent event{QEvent::PaletteChange};
    QCoreApplication::sendEvent(_exampleViewHeaderWidget, &event);
    _exampleViewContentWidget->updateStyleRequested();
}

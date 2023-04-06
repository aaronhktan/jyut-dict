#include "magnifyscrollarea.h"

#include "logic/entry/entry.h"
#include "logic/settings/settingsutils.h"

#include <QCoreApplication>
#include <QScrollBar>

MagnifyScrollArea::MagnifyScrollArea(QWidget *parent)
    : QScrollArea(parent)
{
    setFrameShape(QFrame::NoFrame);

    _settings = Settings::getSettings(this);

    _scrollAreaWidget = new MagnifyScrollAreaWidget{this};

    setWidget(_scrollAreaWidget);
    setWidgetResizable(
        true); // IMPORTANT! This makes the scrolling widget resize correctly.

    setMinimumSize(QSize{500, 500});
}

void MagnifyScrollArea::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    }
}

void MagnifyScrollArea::setEntry(const Entry &entry)
{
    _entry = entry;
    _entryIsValid = true;

    _scrollAreaWidget->setEntry(_entry);
    _scrollAreaWidget->setMaximumWidth(
        width()
        - (verticalScrollBar()->isVisible() ? verticalScrollBar()->width() : 0));
}

void MagnifyScrollArea::resizeEvent(QResizeEvent *event)
{
    event->accept();
    _scrollAreaWidget->setMaximumWidth(
        width()
        - (verticalScrollBar()->isVisible() ? verticalScrollBar()->width() : 0));
    _scrollAreaWidget->resize(width()
                                  - (verticalScrollBar()->isVisible()
                                         ? verticalScrollBar()->width()
                                         : 0),
                              _scrollAreaWidget->sizeHint().height());
}

void MagnifyScrollArea::updateStyleRequested(void)
{
    if (_entryIsValid) {
        _entry.refreshColours(
            _settings
                ->value("entryColourPhoneticType",
                        QVariant::fromValue(EntryColourPhoneticType::CANTONESE))
                .value<EntryColourPhoneticType>());

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
        _entry.generatePhonetic(cantoneseOptions, mandarinOptions);
        _scrollAreaWidget->setEntry(_entry);
    }

    QEvent event{QEvent::PaletteChange};
    QCoreApplication::sendEvent(_scrollAreaWidget, &event);
    _scrollAreaWidget->resize(width()
                                  - (verticalScrollBar()->isVisible()
                                         ? verticalScrollBar()->width()
                                         : 0),
                              _scrollAreaWidget->sizeHint().height());
}

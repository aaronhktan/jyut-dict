#include "magnifyscrollarea.h"

#include "components/magnifywindow/magnifyscrollareawidget.h"
#include "logic/settings/settingsutils.h"

#include <QCoreApplication>
#include <QResizeEvent>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QWidget>

MagnifyScrollArea::MagnifyScrollArea(QWidget *parent)
    : QScrollArea{parent}
    , _settings{Settings::getSettings(this)}
    , _scrollAreaWidget{new MagnifyScrollAreaWidget{this}}
{
    setFrameShape(QFrame::NoFrame);

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

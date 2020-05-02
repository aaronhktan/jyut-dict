#include "resultlistview.h"

#include "components/resultlistmodel.h"
#include "logic/search/sqlsearch.h"

#include <QGuiApplication>

#ifdef Q_OS_WIN
#include <QScrollBar>
#endif

ResultListView::ResultListView(std::shared_ptr<SQLSearch> sqlSearch,
                               QWidget *parent)
    : QListView(parent)
{
    setFrameShape(QFrame::NoFrame);
#ifdef Q_OS_LINUX
    setMinimumWidth(200);
#else
    setMinimumWidth(250);
#endif

    _model = new ResultListModel{sqlSearch, {}, this};
    setModel(_model);

    _delegate = new ResultListDelegate{this};
    setItemDelegate(_delegate);

    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    connect(qApp,
            &QGuiApplication::applicationStateChanged,
            this,
            &ResultListView::paintWithApplicationState);
}

void ResultListView::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        static_cast<ResultListModel *>(_model)->setWelcome();
    }
    QListView::changeEvent(event);
}

// On Windows, because of a bug in Qt (see QTBUG-7232), every time mouse
// is scrolled, listview advances by by three items. Override the wheelEvent to
// modify this undesired behaviour until fixed by Qt.
#ifdef Q_OS_WIN
void ResultListView::wheelEvent(QWheelEvent *event)
{
    int singleStep = verticalScrollBar()->singleStep();
    singleStep = qMin(singleStep, 10);
    verticalScrollBar()->setSingleStep(singleStep);
    QAbstractItemView::wheelEvent(event);
}
#endif

void ResultListView::paintWithApplicationState()
{
    viewport()->update(); // Forces repaint of viewing area.
}

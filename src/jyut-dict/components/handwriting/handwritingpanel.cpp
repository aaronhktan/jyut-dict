#include "handwritingpanel.h"

#include <QStyleOption>

HandwritingPanel::HandwritingPanel(QWidget *parent)
    : QWidget{parent}
{
    _pixmap.setDevicePixelRatio(devicePixelRatio());
#ifdef Q_OS_MAC
    _pixmap.fill(palette().window().color());
#else
    _pixmap.fill(palette().base().color());
#endif

    setFixedSize(350, 350);
}

void HandwritingPanel::clearPanel(void)
{
#ifdef Q_OS_MAC
    _pixmap.fill(palette().window().color());
#else
    _pixmap.fill(palette().base().color());
#endif
    update();
}

void HandwritingPanel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        return;
    }

    _lastPos = event->pos();
    emit strokeStart(event->pos().x() * devicePixelRatio(),
                     event->pos().y() * devicePixelRatio());
    draw(event->pos());
}

void HandwritingPanel::mouseMoveEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        return;
    }

    if (std::abs(event->pos().x() - _lastPos.x()) > 5
        || (std::abs(event->pos().y() - _lastPos.y()) > 5)) {
        emit strokeUpdate(event->pos().x() * devicePixelRatio(),
                          event->pos().y() * devicePixelRatio());
    }

    draw(event->pos());
}

void HandwritingPanel::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        return;
    }

    emit strokeComplete(event->pos().x() * devicePixelRatio(),
                        event->pos().y() * devicePixelRatio());
}

void HandwritingPanel::paintEvent(QPaintEvent *event)
{
    QPainter painter{this};
    painter.drawPixmap(0, 0, _pixmap);

    QStyleOption opt;
    opt.initFrom(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

void HandwritingPanel::resizeEvent(QResizeEvent *event)
{
    auto newRect = _pixmap.rect().united(rect());
    if (newRect == _pixmap.rect()) {
        return;
    }

    QPixmap newPixmap{newRect.size() * devicePixelRatio()};
    QPainter painter{&newPixmap};
#ifdef Q_OS_MAC
    painter.fillRect(newPixmap.rect(), palette().window().color());
#else
    painter.fillRect(newPixmap.rect(), palette().base().color());
#endif
    painter.drawPixmap(0, 0, _pixmap);
    _pixmap = newPixmap;
    _pixmap.setDevicePixelRatio(devicePixelRatio());

    emit pixmapDimensions(_pixmap.width(), _pixmap.height());
}

void HandwritingPanel::draw(const QPoint &pos)
{
    if (pos != _lastPos) {
        QPainter painter{&_pixmap};
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen({palette().text().color(), 4.0});
        painter.drawLine(_lastPos, pos);
        _lastPos = pos;
        update();
    }
}

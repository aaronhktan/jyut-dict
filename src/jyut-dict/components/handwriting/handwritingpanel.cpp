#include "handwritingpanel.h"

HandwritingPanel::HandwritingPanel(QWidget *parent)
    : QWidget{parent}
{
    _pixmap.setDevicePixelRatio(devicePixelRatio());
    _pixmap.fill(palette().base().color());

    setMinimumSize(350, 350);
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
}

void HandwritingPanel::resizeEvent(QResizeEvent *event)
{
    auto newRect = _pixmap.rect().united(rect());
    if (newRect == _pixmap.rect()) {
        return;
    }

    QPixmap newPixmap{newRect.size() * devicePixelRatio()};
    QPainter painter{&newPixmap};
    painter.fillRect(newPixmap.rect(), palette().base().color());
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

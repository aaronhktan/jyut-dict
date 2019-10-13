#include "resizeablelabel.h"

ResizeableLabel::ResizeableLabel(QWidget *parent)
    : QLabel(parent)
{
    setWordWrap(true);
}

QSize ResizeableLabel::sizeHint() const
{
    return QSize{width(),
                 heightForWidth(width())};
}

#include <QResizeEvent>
void ResizeableLabel::resizeEvent(QResizeEvent *event)
{
    setFixedWidth(event->size().width());
}

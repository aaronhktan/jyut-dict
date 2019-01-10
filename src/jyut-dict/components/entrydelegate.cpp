#include "entrydelegate.h"

#include "logic/entry/entry.h"

#include <QDebug>

EntryDelegate::EntryDelegate(QWidget *parent)
    : QStyledItemDelegate (parent)
{
}

void EntryDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.data().canConvert<Entry>()) {
        return;
    }

    painter->save();

    Entry entry = qvariant_cast<Entry>(index.data());

    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    } else {
        painter->fillRect(option.rect, option.palette.base());
    }

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(QPen(option.palette.color(QPalette::WindowText)));

    QRect r = option.rect;
    r = option.rect.adjusted(11, 11, -11, 0);
    QRect boundingRect;
    QFont font = painter->font();
    font.setPixelSize(16);
    painter->setFont(font);
    painter->drawText(r, 0, entry.getSimplified().c_str(), &boundingRect);

    r = r.adjusted(0, boundingRect.height(), 0, 0);
    font.setPixelSize(12);
    painter->setFont(font);
    QFontMetrics metrics(font);
    QString pinyin = metrics.elidedText(entry.getPinyin().c_str(), Qt::ElideRight, r.width());
    painter->drawText(r, 0, pinyin, &boundingRect);

    r = r.adjusted(0, boundingRect.height(), 0, 0);
    QString snippet = metrics.elidedText(entry.getDefinitionSnippet().c_str(), Qt::ElideRight, r.width());
    painter->drawText(r, 0, snippet, &boundingRect);

    QRect rct = option.rect;
    rct.setY(rct.bottom() - 1);
    painter->fillRect(rct, option.palette.color(QPalette::Window));

    painter->restore();
}

QSize EntryDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(100, 80);
}

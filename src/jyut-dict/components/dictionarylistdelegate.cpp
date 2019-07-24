#include "dictionarylistdelegate.h"

#include "logic/entry/dictionarysource.h"

DictionaryListDelegate::DictionaryListDelegate(QWidget *parent)
    : QStyledItemDelegate(parent)
{

}

void DictionaryListDelegate::paint(QPainter *painter,
                                   const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    if (!index.data().canConvert<DictionarySource>()) {
        return;
    }

    painter->save();

    DictionarySource source = qvariant_cast<DictionarySource>(index.data());

    if (option.state & QStyle::State_Selected) {
#ifdef Q_OS_MAC
        painter->fillRect(option.rect, option.palette.highlight());
#else
        painter->fillRect(option.rect, QColor(204, 0, 1));
#endif
        painter->setPen(QPen(option.palette.color(QPalette::HighlightedText)));
    } else {
        painter->fillRect(option.rect, option.palette.base());
        painter->setPen(QPen(option.palette.color(QPalette::WindowText)));
    }

    painter->setRenderHint(QPainter::Antialiasing, true);

    QRect r = option.rect;
    QRect boundingRect;
    QFont font = painter->font();

    r = r.adjusted(6, 6, 6, 6);
    QFontMetrics metrics(font);
    QString phonetic = metrics.elidedText(
        DictionarySourceUtils::getSourceLongString(source).c_str(),
        Qt::ElideRight,
        r.width());
    painter->drawText(r, 0, phonetic, &boundingRect);

    painter->restore();
}

QSize DictionaryListDelegate::sizeHint(const QStyleOptionViewItem &option,
                                       const QModelIndex &index) const
{
    return QSize(100, 30);
}

#include "dictionarylistdelegate.h"

#include "logic/dictionary/dictionarymetadata.h"
#include "logic/utils/utils.h"
#include "logic/utils/utils_qt.h"

#include <QGuiApplication>

DictionaryListDelegate::DictionaryListDelegate(QWidget *parent)
    : QStyledItemDelegate(parent)
{

}

void DictionaryListDelegate::paint(QPainter *painter,
                                   const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    if (!index.data().canConvert<DictionaryMetadata>()) {
        return;
    }

    painter->save();

    DictionaryMetadata source = qvariant_cast<DictionaryMetadata>(index.data());

    QColor backgroundColour;
    if (option.state & QStyle::State_Selected) {
        if (QGuiApplication::applicationState() == Qt::ApplicationInactive) {
            backgroundColour = option.palette
                                   .brush(QPalette::Inactive,
                                          QPalette::Highlight)
                                   .color();
        } else {
            backgroundColour = option.palette
                                   .brush(QPalette::Active,
                                          QPalette::Highlight)
                                   .color();
        }
        painter->fillRect(option.rect, backgroundColour);
        QColor textColour = Utils::getContrastingColour(backgroundColour);
        painter->setPen(textColour);
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
    QString sourcename = metrics.elidedText(
        source.getName().c_str(),
        Qt::ElideRight,
        r.width());
    painter->drawText(r, 0, sourcename, &boundingRect);

    painter->restore();
}

QSize DictionaryListDelegate::sizeHint(const QStyleOptionViewItem &option,
                                       const QModelIndex &index) const
{
    (void) (option);
    (void) (index);
    return QSize(100, 30);
}

#include "searchhistorylistdelegate.h"

#include "logic/database/sqluserhistoryutils.h"
#include "logic/settings/settingsutils.h"
#include "logic/utils/utils.h"
#include "logic/utils/utils_qt.h"

#include <QGuiApplication>
#include <QVariant>

#include <string>
#include <utility>

SearchHistoryListDelegate::SearchHistoryListDelegate(QWidget *parent)
    : QStyledItemDelegate (parent)
{
    _settings = Settings::getSettings(this);
}

void SearchHistoryListDelegate::paint(QPainter *painter,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index) const
{
    if (!index.data().canConvert<searchTermHistoryItem>()) {
        return;
    }

    painter->save();

    std::pair<std::string, int> pair
        = qvariant_cast<std::pair<std::string, int>>(index.data());

    bool isEmptyPair = (pair.second == -1);

    QColor backgroundColour;
    if (option.state & QStyle::State_Selected && !isEmptyPair) {
        if (QGuiApplication::applicationState() == Qt::ApplicationInactive) {
#ifdef Q_OS_MAC
            backgroundColour = option.palette
                                   .brush(QPalette::Inactive,
                                          QPalette::Highlight)
                                   .color();
#else
            backgroundColour = QColor{LIST_ITEM_INACTIVE_COLOUR_LIGHT_R,
                                      LIST_ITEM_INACTIVE_COLOUR_LIGHT_G,
                                      LIST_ITEM_INACTIVE_COLOUR_LIGHT_B};
#endif
        } else {
#ifdef Q_OS_MAC
            backgroundColour = option.palette
                                   .brush(QPalette::Active,
                                          QPalette::Highlight)
                                   .color();
#else
            backgroundColour = QColor{LIST_ITEM_ACTIVE_COLOUR_LIGHT_R,
                                      LIST_ITEM_ACTIVE_COLOUR_LIGHT_G,
                                      LIST_ITEM_ACTIVE_COLOUR_LIGHT_B};
#endif
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
    QString searchTerm = metrics.elidedText(pair.first.c_str(),
                                            Qt::ElideRight,
                                            r.width());
    painter->drawText(r, 0, searchTerm, &boundingRect);

    // Bottom divider
    QRect rct = option.rect;
    rct.setY(rct.bottom() - 1);
    painter->fillRect(rct, option.palette.color(QPalette::Window));

    painter->restore();
}

QSize SearchHistoryListDelegate::sizeHint(const QStyleOptionViewItem &option,
                                          const QModelIndex &index) const
{
    (void) (option);

    std::pair<std::string, int> pair
        = qvariant_cast<searchTermHistoryItem>(index.data());
    bool isEmptyPair = (pair.second == -1);

    if (isEmptyPair) {
#ifdef Q_OS_MAC
        return QSize(100, 130);
#else
        return QSize(100, 135);
#endif
    } else {
        return QSize(100, 30);
    }
}

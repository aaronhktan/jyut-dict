#include "searchhistorylistdelegate.h"

#include "logic/database/sqluserhistoryutils.h"
#include "logic/settings/settingsutils.h"
#include "logic/utils/utils.h"
#include "logic/utils/utils_qt.h"

#include <QGuiApplication>
#include <QTextLayout>
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

    searchTermHistoryItem pair
        = qvariant_cast<searchTermHistoryItem>(index.data());

    // Use -1 to indicate that this is not a valid history item
    bool isEmptyPair = (pair.second == -1);

    QColor backgroundColour;
    if (option.state & QStyle::State_Selected && !isEmptyPair) {
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
        painter->fillRect(option.rect, option.palette.window());
        painter->setPen(QPen(option.palette.color(QPalette::WindowText)));
    }

    painter->setRenderHint(QPainter::Antialiasing, true);

    QRect r = option.rect;
    QRect boundingRect;
    QFont font = painter->font();

#ifdef Q_OS_WIN
    QFont oldFont{font};
    font = QFont("Microsoft Yahei");
#endif

    if (isEmptyPair) {
        // In the empty pair, we paint a big title, a separator, and a bunch of
        // lines with lots of detail.
        font.setPixelSize(20);
        painter->setFont(font);
        r = option.rect.adjusted(11, 11, -11, 0); // 11 specifies the margin
        painter->drawText(r, 0, pair.first.c_str());

#ifdef Q_OS_WIN
        font = oldFont;
#endif
        QFontMetrics metrics{font};
        font.setPixelSize(14);
        painter->setFont(font);
        r = r.adjusted(0, 28, 0, 0);
        metrics = QFontMetrics(font);
        QString phonetic = "—";
        painter->drawText(r, 0, phonetic, &boundingRect);

        // Add some extra spacing if text is not in Chinese; Chinese text
        // overhangs the baseline by a few pixels.
        if (Settings::isCurrentLocaleHan()) {
            r = r.adjusted(0, boundingRect.height() + 5, 0, 0);
            font.setPixelSize(13);
        } else {
            r = r.adjusted(0, boundingRect.height() + 10, 0, 0);
            font.setPixelSize(11);
        }
        painter->setFont(font);
        painter->save();
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        painter->setPen(QPen(option.palette.color(QPalette::PlaceholderText)));
#else
        painter->setPen(QPen(option.palette.color(QPalette::Disabled, QPalette::WindowText)));
#endif

        // Do custom text layout to get eliding double-line label
        QString snippet = tr("After searching for a word, you will find it "
                             "in this list. Selecting a word will allow you "
                             "to do the same search again.");
        QTextLayout *textLayout = new QTextLayout{snippet, painter->font()};
        textLayout->beginLayout();

        // Define start and end y coordinates
        // max height of label is three lines, so height * 3
        int y = r.y();
        int height = y + metrics.height() * 3;

        for (;;) {
            QTextLine line = textLayout->createLine();

            if (!line.isValid()) {
                break;
            }

            line.setLineWidth(r.width());
            int nextLineY = y + metrics.lineSpacing();

            if (height >= nextLineY + metrics.lineSpacing()) {
                line.draw(painter, QPoint(r.x(), y));
                y = nextLineY;
            } else {
                QString lastLine = snippet.mid(line.textStart());
                QString elidedLastLine = metrics.elidedText(lastLine,
                                                            Qt::ElideRight,
                                                            r.width());
                // For some reason at small font sizes, -4 is necessary to make
                // it look right (except in Chinese fonts). *shrug*
                if (Settings::isCurrentLocaleHan()) {
                    painter->drawText(QPoint(r.x(), y + metrics.ascent()),
                                      elidedLastLine);
                } else {
                    painter->drawText(QPoint(r.x(), y + metrics.ascent() - 4),
                                      elidedLastLine);
                }
                line = textLayout->createLine();
                break;
            }
        }

        textLayout->endLayout();
        delete textLayout;

        painter->restore();
    } else {
        font.setPixelSize(16);
        painter->setFont(font);
        int margin = 6;
        r = r.adjusted(margin, margin, margin, margin);

        QFontMetrics metrics(font);
        QString searchOption = Utils::getStringFromSearchParameter(
                                   static_cast<SearchParameters>(pair.second))
                                   .c_str();
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        int searchOptionWidth = metrics.horizontalAdvance(searchOption);
#else
        int searchOptionWidth = metrics.width(searchOption);
#endif
        QString searchTerm = metrics.elidedText(pair.first.c_str(),
                                                Qt::ElideRight,
                                                r.width() - 2 * margin
                                                    - searchOptionWidth);
        QRect rectangle = r;
        rectangle.setHeight(metrics.height());
        painter->drawText(rectangle, 0, searchTerm, &boundingRect);

        r.setX(r.width() - margin - searchOptionWidth);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        painter->setPen(QPen(option.palette.color(QPalette::PlaceholderText)));
#else
        painter->setPen(QPen(option.palette.color(QPalette::Disabled, QPalette::WindowText)));
#endif
        painter->drawText(r, 0, searchOption, &boundingRect);
    }

    // Bottom divider
    QRect rct = option.rect;
    rct.setY(rct.bottom() - 1);
    painter->fillRect(rct, option.palette.alternateBase());

    painter->restore();
}

QSize SearchHistoryListDelegate::sizeHint(const QStyleOptionViewItem &option,
                                          const QModelIndex &index) const
{
    (void) (option);

    searchTermHistoryItem pair
        = qvariant_cast<searchTermHistoryItem>(index.data());
    bool isEmptyPair = (pair.second == -1);

    if (isEmptyPair) {
#ifdef Q_OS_MAC
        return QSize(100, 130);
#else
        return QSize(100, 135);
#endif
    } else {
#ifdef Q_OS_MAC
        return QSize(100, 32);
#else
        return QSize(100, 37);
#endif
    }
}

#include "searchhistorylistdelegate.h"

#include "logic/database/sqluserhistoryutils.h"
#include "logic/settings/settings.h"
#include "logic/settings/settingsutils.h"
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
        QColor textColour{Utils::getContrastingColour(backgroundColour)};
        painter->setPen(textColour);
    } else {
        painter->fillRect(option.rect, option.palette.base());
        painter->setPen(QPen(option.palette.color(QPalette::WindowText)));
    }

    painter->setRenderHint(QPainter::Antialiasing, true);

    QRect r = option.rect;
    QRect boundingRect;
    QFont font = painter->font();
    int interfaceSize = static_cast<int>(
        _settings
            ->value("Interface/size",
                    QVariant::fromValue(Settings::InterfaceSize::NORMAL))
            .value<Settings::InterfaceSize>());
    int h4FontSize = Settings::h4FontSize.at(
        static_cast<unsigned long>(interfaceSize - 1));
    int bodyFontSize = Settings::bodyFontSize.at(
        static_cast<unsigned long>(interfaceSize - 1));
    int bodyFontSizeHan = Settings::bodyFontSizeHan.at(
        static_cast<unsigned long>(interfaceSize - 1));
    int cellTopPadding = bodyFontSize * 2 / 3;
    int cellLeftPadding = bodyFontSize * 2 / 3;
    int contentSpacingMargin = bodyFontSize / 2;

#ifdef Q_OS_WIN
    QFont oldFont{font};
    font = QFont("Microsoft Yahei");
#endif

    if (isEmptyPair) {
        // In the empty pair, we paint a big title, a separator, and a bunch of
        // lines with lots of detail.
        font.setPixelSize(h4FontSize);
        painter->setFont(font);
        r = option.rect.adjusted(cellTopPadding,
                                 cellLeftPadding,
                                 -cellLeftPadding,
                                 0);
        painter->drawText(r, 0, pair.first.c_str());
        r = r.adjusted(0, h4FontSize + contentSpacingMargin * 2, 0, 0);

#ifdef Q_OS_WIN
        font = oldFont;
#endif
        font.setPixelSize(bodyFontSize + 2);
        painter->setFont(font);
        QFontMetrics metrics{font};
        QString phonetic = "—";
        painter->drawText(r, 0, phonetic, &boundingRect);
        r = r.adjusted(0, bodyFontSize + 2 + contentSpacingMargin, 0, 0);

        if (Settings::isCurrentLocaleHan()) {
            font.setPixelSize(bodyFontSizeHan);
        } else {
            font.setPixelSize(bodyFontSize);
        }
        painter->setFont(font);
        painter->setPen(QPen(option.palette.color(QPalette::PlaceholderText)));

        // Do custom text layout to get eliding double-line label
        QString snippet = tr("After searching for a word, you will find it "
                             "in this list. Selecting a word will allow you "
                             "to do the same search again.");
        QTextLayout *textLayout = new QTextLayout{snippet, painter->font()};
        textLayout->beginLayout();

        // Define start and end y coordinates
        // max height of label is four lines, so height * 4
        int y = r.y();
        int height = y + metrics.height() * 4;

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
    } else {
        font.setPixelSize(bodyFontSize);
        painter->setFont(font);
        r = r.adjusted(cellTopPadding,
                       cellLeftPadding,
                       -cellTopPadding,
                       -cellLeftPadding);

        QFontMetrics metrics{font};
        QString searchOption = Utils::getStringFromSearchParameter(
                                   static_cast<SearchParameters>(pair.second))
                                   .c_str();
        int searchOptionWidth = metrics.horizontalAdvance(searchOption);
        QString searchTerm = metrics.elidedText(pair.first.c_str(),
                                                Qt::ElideRight,
                                                r.width() - 2 * cellLeftPadding
                                                    - searchOptionWidth);
        QRect rectangle = r;
        rectangle.setHeight(metrics.height());
        painter->drawText(rectangle, 0, searchTerm, &boundingRect);

        r.setX(r.width() - searchOptionWidth);
        if (option.state & QStyle::State_Selected) {
            QColor textColour{Utils::getContrastingColour(backgroundColour)};
            painter->setPen(textColour);
        } else {
            painter->setPen(QPen(option.palette.color(QPalette::PlaceholderText)));
        }
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

    Settings::InterfaceSize interfaceSize
        = _settings
              ->value("Interface/size",
                      QVariant::fromValue(Settings::InterfaceSize::NORMAL))
              .value<Settings::InterfaceSize>();

    if (isEmptyPair) {
#ifdef Q_OS_MAC
        switch (interfaceSize) {
        case Settings::InterfaceSize::SMALLER: {
            switch (Settings::getCurrentLocale().language()) {
            case QLocale::English: {
                return QSize(100, 100);
            }
            default: {
                return QSize(100, 90);
            }
            }
        }
        case Settings::InterfaceSize::SMALL: {
            return QSize(100, 110);
        }
        case Settings::InterfaceSize::NORMAL: {
            return QSize(100, 125);
        }
        case Settings::InterfaceSize::LARGE: {
            return QSize(100, 145);
        }
        case Settings::InterfaceSize::LARGER: {
            return QSize(100, 185);
        }
        }
#else
        switch (interfaceSize) {
        case Settings::InterfaceSize::SMALLER: {
            return QSize(100, 95);
        }
        case Settings::InterfaceSize::SMALL: {
            switch (Settings::getCurrentLocale().language()) {
            case QLocale::Cantonese:
            case QLocale::Chinese:
            case QLocale::French: {
                return QSize(100, 100);
            }
            default: {
                return QSize(100, 120);
            }
            }
        }
        case Settings::InterfaceSize::NORMAL: {
            return QSize(100, 135);
        }
        case Settings::InterfaceSize::LARGE: {
            return QSize(100, 155);
        }
        case Settings::InterfaceSize::LARGER: {
            switch (Settings::getCurrentLocale().language()) {
            case QLocale::French:
            case QLocale::Chinese:
            case QLocale::Cantonese: {
                return QSize(100, 180);
            }
            default: {
                return QSize(100, 200);
            }
            }
        }
        }
#endif
    } else {
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
        switch (interfaceSize) {
        case Settings::InterfaceSize::SMALLER: {
            return QSize(100, 30);
        }
        case Settings::InterfaceSize::SMALL: {
            return QSize(100, 35);
        }
        case Settings::InterfaceSize::NORMAL: {
            return QSize(100, 40);
        }
        case Settings::InterfaceSize::LARGE: {
            return QSize(100, 45);
        }
        case Settings::InterfaceSize::LARGER: {
            return QSize(100, 50);
        }
        }
#else
        switch (interfaceSize) {
        case Settings::InterfaceSize::SMALLER: {
            return QSize(100, 25);
        }
        case Settings::InterfaceSize::SMALL: {
            return QSize(100, 30);
        }
        case Settings::InterfaceSize::NORMAL: {
            return QSize(100, 35);
        }
        case Settings::InterfaceSize::LARGE: {
            return QSize(100, 40);
        }
        case Settings::InterfaceSize::LARGER: {
            return QSize(100, 45);
        }
        }
#endif
    }

    // All cases should be handled and the function should
    // never reach here.
    return QSize(100, 100);
}

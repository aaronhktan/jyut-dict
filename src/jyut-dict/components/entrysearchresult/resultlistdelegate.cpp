#include "resultlistdelegate.h"

#include "logic/entry/entry.h"
#include "logic/entry/entrycharactersoptions.h"
#include "logic/entry/entryphoneticoptions.h"
#include "logic/settings/settingsutils.h"
#include "logic/utils/utils.h"
#include "logic/utils/utils_qt.h"

#include <QGuiApplication>
#include <QAbstractTextDocumentLayout>
#include <QRectF>
#include <QTextDocument>
#include <QTextLayout>
#include <QVariant>

ResultListDelegate::ResultListDelegate(QWidget *parent)
    : QStyledItemDelegate (parent)
{
    _settings = Settings::getSettings(this);
}

void ResultListDelegate::paint(QPainter *painter,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index) const
{
    if (!index.data().canConvert<Entry>()) {
        return;
    }

    painter->save();

    Entry entry = qvariant_cast<Entry>(index.data());

    bool isWelcomeEntry = entry.isWelcome();
    bool isEmptyEntry = entry.isEmpty();

    QColor backgroundColour;
    if (option.state & QStyle::State_Selected && !isWelcomeEntry && !isEmptyEntry) {
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

    EntryCharactersOptions characterOptions;
    EntryPhoneticOptions phoneticOptions;
    MandarinOptions mandarinOptions;
    bool use_colours = false;
    if (isWelcomeEntry || isEmptyEntry) {
        characterOptions = EntryCharactersOptions::ONLY_SIMPLIFIED;
        phoneticOptions = EntryPhoneticOptions::ONLY_PINYIN;
        mandarinOptions = MandarinOptions::RAW_PINYIN;
    } else {
        characterOptions
            = _settings
                  ->value("characterOptions",
                          QVariant::fromValue(
                              EntryCharactersOptions::PREFER_TRADITIONAL))
                  .value<EntryCharactersOptions>();
        phoneticOptions = _settings
                              ->value("phoneticOptions",
                                      QVariant::fromValue(
                                          EntryPhoneticOptions::PREFER_JYUTPING))
                              .value<EntryPhoneticOptions>();
        mandarinOptions = _settings
                              ->value("mandarinOptions",
                                      QVariant::fromValue(
                                          MandarinOptions::PRETTY_PINYIN))
                              .value<MandarinOptions>();

        use_colours = !(option.state & QStyle::State_Selected);
    }

    QRect r = option.rect;
    QRect boundingRect;
    QFont font = painter->font();

    // Chinese characters
#ifdef Q_OS_WIN
    QFont oldFont = font;
    font = QFont("Microsoft Yahei");
#endif
    font.setPixelSize(20);
    painter->setFont(font);
    r = option.rect.adjusted(11, 11, -11, 0);
    QFontMetrics metrics(font);

    // Use QTextDocument for rich text
    QTextDocument *doc = new QTextDocument{};
    entry.refreshColours(_settings
                             ->value("entryColourPhoneticType",
                                     QVariant::fromValue(
                                         EntryColourPhoneticType::JYUTPING))
                             .value<EntryColourPhoneticType>());
    doc->setHtml(QString(entry.getCharacters(characterOptions, use_colours).c_str()));
    doc->setTextWidth(r.width());
    doc->setDefaultFont(font);
    doc->setDocumentMargin(0);
    QAbstractTextDocumentLayout *documentLayout = doc->documentLayout();
    auto ctx = QAbstractTextDocumentLayout::PaintContext();
    ctx.palette.setColor(QPalette::Text, painter->pen().color());
    QRectF bounds = QRectF(0, 0, r.width(), 16);
    ctx.clip = bounds;
    painter->translate(11, r.y());
    documentLayout->draw(painter, ctx);
    painter->translate(-11, -r.y());

    delete doc;

    // Phonetic and definition snippets
#ifdef Q_OS_WIN
    font = oldFont;
#endif
    QString snippet;
    if (isEmptyEntry) {
        font.setPixelSize(14);
        painter->setFont(font);
        r = r.adjusted(0, 28, 0, 0);
        metrics = QFontMetrics(font);
        QString phonetic = metrics.elidedText(entry.getJyutping().c_str(),
                                              Qt::ElideRight,
                                              r.width());
        painter->drawText(r, 0, phonetic, &boundingRect);

        if (Settings::isCurrentLocaleHan()) {
            r = r.adjusted(0, boundingRect.height() + 5, 0, 0);
            font.setPixelSize(13);
        } else {
            r = r.adjusted(0, boundingRect.height() + 10, 0, 0);
            font.setPixelSize(11);
        }
        painter->setFont(font);
        painter->save();
        painter->setPen(QPen(option.palette.color(QPalette::PlaceholderText)));

        // Do custom text layout to get eliding double-line label
        snippet = entry.getDefinitionSnippet().c_str();
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
        font.setPixelSize(12);
        painter->setFont(font);
        r = r.adjusted(0, 30, 0, 0);
        metrics = QFontMetrics(font);
        QString phonetic = metrics.elidedText(
            entry.getPhonetic(phoneticOptions, mandarinOptions).c_str(),
            Qt::ElideRight,
            r.width());
        painter->drawText(r, 0, phonetic, &boundingRect);
        r = r.adjusted(0, boundingRect.height(), 0, 0);

        snippet = metrics.elidedText(
            entry.getDefinitionSnippet().c_str(),
            Qt::ElideRight, r.width());
        painter->drawText(r, 0, snippet, &boundingRect);
    }

    // Bottom divider
    QRect rct = option.rect;
    rct.setY(rct.bottom() - 1);
    painter->fillRect(rct, option.palette.color(QPalette::Window));

    painter->restore();
}

QSize ResultListDelegate::sizeHint(const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    Entry entry = qvariant_cast<Entry>(index.data());
    bool isEmptyEntry = entry.isEmpty();

    if (isEmptyEntry) {
#ifdef Q_OS_MAC
        return QSize(100, 130);
#else
        return QSize(100, 135);
#endif
    } else {
#ifdef Q_OS_LINUX
        return QSize(100, 90);
#else
        return QSize(100, 85);
#endif
    }
}
#include "sentenceresultlistdelegate.h"

#include "logic/entry/entrycharactersoptions.h"
#include "logic/entry/entryphoneticoptions.h"
#include "logic/sentence/sourcesentence.h"
#include "logic/settings/settingsutils.h"
#include "logic/utils/utils.h"
#include "logic/utils/utils_qt.h"

#include <QGuiApplication>
#include <QAbstractTextDocumentLayout>
#include <QLocale>
#include <QRectF>
#include <QTextDocument>
#include <QTextLayout>
#include <QVariant>

SentenceResultListDelegate::SentenceResultListDelegate(QWidget *parent)
    : QStyledItemDelegate (parent)
{
    _settings = Settings::getSettings(this);
}

void SentenceResultListDelegate::paint(QPainter *painter,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index) const
{
    if (!index.data().canConvert<SourceSentence>()) {
        return;
    }

    painter->save();

    SourceSentence sentence = qvariant_cast<SourceSentence>(index.data());

    QColor backgroundColour;
    if (option.state & QStyle::State_Selected) {
#ifdef Q_OS_MAC
        backgroundColour = option.palette
                               .brush(QPalette::Active, QPalette::Highlight)
                               .color();
#else
        backgroundColour = QColor{LIST_ITEM_ACTIVE_COLOUR_LIGHT_R,
                                  LIST_ITEM_ACTIVE_COLOUR_LIGHT_G,
                                  LIST_ITEM_ACTIVE_COLOUR_LIGHT_B};
#endif
        painter->fillRect(option.rect, backgroundColour);
    } else {
        painter->fillRect(option.rect, option.palette.base());
    }

    painter->setRenderHint(QPainter::Antialiasing, true);

    EntryCharactersOptions characterOptions;
    EntryPhoneticOptions phoneticOptions;
    CantoneseOptions cantoneseOptions;
    MandarinOptions mandarinOptions;

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
    cantoneseOptions = _settings
                          ->value("cantoneseOptions",
                                  QVariant::fromValue(
                                      CantoneseOptions::RAW_JYUTPING))
                          .value<CantoneseOptions>();
    mandarinOptions = _settings
                          ->value("mandarinOptions",
                                  QVariant::fromValue(
                                      MandarinOptions::PRETTY_PINYIN))
                          .value<MandarinOptions>();

    QRect r = option.rect;
    QRect boundingRect;
    QFont font = painter->font();
    QFontMetrics metrics{font};

    // Draw language indicator
    std::string language = sentence.getSourceLanguage();
    QColor colour = Utils::getLanguageColour(language);
    r = r.adjusted(15, 11, 0, 0);

    painter->save();
    font.setPixelSize(12);
    painter->setFont(font);
    metrics = QFontMetrics(font);
    QString languageString = Utils::getLanguageFromISO639(language);
    languageString = metrics
                         .elidedText(languageString, Qt::ElideRight, r.width())
                         .trimmed();
    painter->drawText(r, 0, languageString, &boundingRect);

    QPainterPath path;
    boundingRect = boundingRect.adjusted(-7, -2, 7, 2);
    path.addRoundedRect(boundingRect, 10, 10);
    if (option.state & QStyle::State_Selected) {
        painter->fillPath(path, backgroundColour.lighter(125));
        painter->setPen(Utils::getContrastingColour(backgroundColour));
    } else {
        painter->fillPath(path, colour);
        painter->setPen(Utils::getContrastingColour(colour));
    }

    painter->drawText(r, 0, languageString, &boundingRect);
    r = r.adjusted(-11, boundingRect.height() + 4, 0, 0);
    painter->restore();

    // Chinese characters
    painter->save();
#ifdef Q_OS_WIN
    QFont oldFont = font;
    font = QFont("Microsoft Yahei");
#endif
    font.setPixelSize(20);
    painter->setFont(font);
    if (option.state &QStyle::State_Selected) {
        painter->setPen(Utils::getContrastingColour(backgroundColour));
    }

    // Use QTextDocument for rich text
    r = option.rect.adjusted(11, 30, -11, 0);
    QTextDocument *doc = new QTextDocument{};
    doc->setHtml(QString(sentence.getCharacters(characterOptions).c_str()));
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
    painter->restore();

    delete doc;

    // Phonetic and definition snippets
#ifdef Q_OS_WIN
    font = oldFont;
#endif
    QString snippet;
    font.setPixelSize(12);
    painter->setFont(font);
    r = r.adjusted(0, 28, 0, 0);
    metrics = QFontMetrics(font);
    painter->save();
    if (option.state & QStyle::State_Selected) {
        painter->setPen(
            Utils::getContrastingColour(backgroundColour).darker(125));
    } else {
        painter->setPen(QPen(option.palette.color(QPalette::PlaceholderText)));
    }
    QString phonetic = metrics
                           .elidedText(sentence
                                           .getPhonetic(phoneticOptions,
                                                        cantoneseOptions,
                                                        mandarinOptions)
                                           .c_str(),
                                       Qt::ElideRight,
                                       r.width())
                           .trimmed();
    painter->drawText(r, 0, phonetic, &boundingRect);
    r = r.adjusted(0, boundingRect.height(), 0, 0);
    painter->restore();

    painter->save();
    if (option.state & QStyle::State_Selected) {
        painter->setPen(Utils::getContrastingColour(backgroundColour));
    }
    r.setY(
        option.rect.bottom() - 10
        - metrics.boundingRect(sentence.getSentenceSnippet().c_str()).height());
    snippet = metrics
                  .elidedText(sentence.getSentenceSnippet().c_str(),
                              Qt::ElideRight,
                              r.width())
                  .trimmed();
    painter->drawText(r, 0, snippet, &boundingRect);
    r = r.adjusted(0, boundingRect.height(), 0, 0);
    painter->restore();

    // Bottom divider
    QRect rct = option.rect;
    rct.setY(rct.bottom() - 1);
    painter->fillRect(rct, option.palette.color(QPalette::Window));

    painter->restore();
}

QSize SentenceResultListDelegate::sizeHint(const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    SourceSentence sentence = qvariant_cast<SourceSentence>(index.data());

#ifdef Q_OS_LINUX
    return QSize(100, 90);
#else
    return QSize(100, 100);
#endif
}

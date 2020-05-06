#include "sentenceresultlistdelegate.h"

#include "logic/entry/entrycharactersoptions.h"
#include "logic/entry/entryphoneticoptions.h"
#include "logic/sentence/sourcesentence.h"
#include "logic/settings/settingsutils.h"
#include "logic/utils/utils.h"
#include "logic/utils/utils_qt.h"

#include <QGuiApplication>
#include <QAbstractTextDocumentLayout>
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

    painter->fillRect(option.rect, option.palette.base());
    painter->setPen(QPen(option.palette.color(QPalette::WindowText)));

    painter->setRenderHint(QPainter::Antialiasing, true);

    EntryCharactersOptions characterOptions;
    EntryPhoneticOptions phoneticOptions;
    CantoneseOptions cantoneseOptions;
    MandarinOptions mandarinOptions;
    bool use_colours = false;

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

    use_colours = !(option.state & QStyle::State_Selected);

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

    delete doc;

    // Phonetic and definition snippets
#ifdef Q_OS_WIN
    font = oldFont;
#endif
    QString snippet;
    font.setPixelSize(12);
    painter->setFont(font);
    r = r.adjusted(0, 30, 0, 0);
    metrics = QFontMetrics(font);
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

    snippet = metrics.elidedText(
        sentence.getSentenceSnippet().c_str(),
                         Qt::ElideRight, r.width()).trimmed();
    painter->drawText(r, 0, snippet, &boundingRect);

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
    return QSize(100, 85);
#endif
}

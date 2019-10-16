#include "resultlistdelegate.h"

#include "logic/entry/entry.h"
#include "logic/entry/entrycharactersoptions.h"
#include "logic/entry/entryphoneticoptions.h"
#include "logic/settings/settingsutils.h"

#include <QGuiApplication>
#include <QAbstractTextDocumentLayout>
#include <QRectF>
#include <QTextDocument>
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

    bool isWelcomeEntry = entry.getSimplified() == tr("Welcome!").toStdString();

    if (option.state & QStyle::State_Selected && !isWelcomeEntry) {
        if (QGuiApplication::applicationState() == Qt::ApplicationInactive) {
#ifdef Q_OS_MAC
            painter->fillRect(option.rect,
                              option.palette.brush(QPalette::Inactive,
                                                   QPalette::Highlight));
#else
            painter->fillRect(option.rect, QColor(70, 70, 70));
#endif
        } else {
#ifdef Q_OS_MAC
            painter->fillRect(option.rect, option.palette.highlight());
#else
            painter->fillRect(option.rect, QColor(204, 0, 1));
#endif
        }
        painter->setPen(QPen(option.palette.color(QPalette::HighlightedText)));
    } else {
        painter->fillRect(option.rect, option.palette.base());
        painter->setPen(QPen(option.palette.color(QPalette::WindowText)));
    }

    painter->setRenderHint(QPainter::Antialiasing, true);

    EntryCharactersOptions characterOptions;
    EntryPhoneticOptions phoneticOptions;
    MandarinOptions mandarinOptions;
    bool use_colours = false;
    if (isWelcomeEntry) {
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
    font.setPixelSize(16);
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
    font.setPixelSize(12);
    painter->setFont(font);

    r = r.adjusted(0, 24, 0, 0);
    metrics = QFontMetrics(font);
    QString phonetic = metrics.elidedText(
                entry.getPhonetic(phoneticOptions,mandarinOptions).c_str(),
                Qt::ElideRight, r.width());
    painter->drawText(r, 0, phonetic, &boundingRect);

    r = r.adjusted(0, boundingRect.height(), 0, 0);
    QString snippet = metrics.elidedText(
                entry.getDefinitionSnippet().c_str(),
                Qt::ElideRight, r.width());
    painter->drawText(r, 0, snippet, &boundingRect);

    // Bottom divider
    QRect rct = option.rect;
    rct.setY(rct.bottom() - 1);
    painter->fillRect(rct, option.palette.color(QPalette::Window));

    painter->restore();
}

QSize ResultListDelegate::sizeHint(const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    return QSize(100, 80);
}

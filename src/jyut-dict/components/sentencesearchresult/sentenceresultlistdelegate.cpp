#include "sentenceresultlistdelegate.h"

#include "logic/entry/entrycharactersoptions.h"
#include "logic/entry/entryphoneticoptions.h"
#include "logic/sentence/sourcesentence.h"
#include "logic/settings/settings.h"
#include "logic/settings/settingsutils.h"
#include "logic/utils/utils_qt.h"

#include <QGuiApplication>
#include <QAbstractTextDocumentLayout>
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
#include <QPainterPath>
#endif
#include <QRectF>
#include <QTextDocument>
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
    // Get the sentence to paint
    if (!index.data().canConvert<SourceSentence>()) {
        return;
    }
    SourceSentence sentence = qvariant_cast<SourceSentence>(index.data());

    painter->save();

    // Draw the rectangle behind each cell in the result view
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
    } else {
        painter->fillRect(option.rect, option.palette.base());
    }

    painter->setRenderHint(QPainter::Antialiasing, true);

    // Get preferences from user
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
                          ->value("Preview/phoneticOptions",
                                  QVariant::fromValue(
                                      EntryPhoneticOptions::PREFER_CANTONESE))
                          .value<EntryPhoneticOptions>();
    cantoneseOptions = _settings
                          ->value("Preview/cantonesePronunciationOptions",
                                  QVariant::fromValue(
                                      CantoneseOptions::RAW_JYUTPING))
                          .value<CantoneseOptions>();
    mandarinOptions = _settings
                          ->value("Preview/mandarinPronunciationOptions",
                                  QVariant::fromValue(
                                      MandarinOptions::PRETTY_PINYIN))
                          .value<MandarinOptions>();
    sentence.generatePhonetic(cantoneseOptions, mandarinOptions);

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
    int sourceLanguageIndicatorHorizontalMargin = 4;
    int cellTopPadding = bodyFontSize * 8 / 6;
    int cellLeftPadding = bodyFontSize;
    int contentSpacingMargin = bodyFontSize / 2;

    // Draw language indicator
    std::string language = sentence.getSourceLanguage();
    QColor colour = Utils::getLanguageColour(language);
    // Adjust a few extra pixels to the left so that the rounded corners of
    // the "pill" look right
    r = r.adjusted(cellLeftPadding + sourceLanguageIndicatorHorizontalMargin,
                   cellTopPadding,
                   -cellLeftPadding,
                   0);

    painter->save();
    font.setPixelSize(bodyFontSize);
    painter->setFont(font);

    // Paint the first time so we can get the bounding rectangle
    QString languageString = Utils::getLanguageFromISO639(language);
    QFontMetrics metrics{font};
    languageString = metrics
                         .elidedText(languageString, Qt::ElideRight, r.width())
                         .trimmed();
    painter->drawText(r, 0, languageString, &boundingRect);

    // Then draw the actual rounded rectangle that contains the language
    QPainterPath path;
    int sourceLanguageIndicatorBorderRadius = bodyFontSize * 7 / 8;
    int sourceLanguageIndicatorVerticalPadding = bodyFontSize / 4;
    int sourceLanguageIndicatorHorizontalPadding = bodyFontSize / 2;
    boundingRect
        = boundingRect.adjusted(-sourceLanguageIndicatorHorizontalPadding,
                                -sourceLanguageIndicatorVerticalPadding,
                                sourceLanguageIndicatorHorizontalPadding,
                                sourceLanguageIndicatorVerticalPadding);
    path.addRoundedRect(boundingRect,
                        sourceLanguageIndicatorBorderRadius,
                        sourceLanguageIndicatorBorderRadius);
    if (option.state & QStyle::State_Selected) {
        painter->fillPath(path, backgroundColour.lighter(125));
        painter->setPen(Utils::getContrastingColour(backgroundColour));
    } else {
        painter->fillPath(path, colour);
        painter->setPen(Utils::getContrastingColour(colour));
    }

    painter->drawText(r, 0, languageString, &boundingRect);
    painter->restore();
    r = r.adjusted(-sourceLanguageIndicatorHorizontalMargin,
                   bodyFontSize + sourceLanguageIndicatorVerticalPadding
                       + contentSpacingMargin,
                   0,
                   0);

    // Chinese characters
    painter->save();
#ifdef Q_OS_WIN
    QFont oldFont = font;
    font = QFont("Microsoft Yahei");
#endif
    font.setPixelSize(h4FontSize);
    painter->setFont(font);
    if (option.state &QStyle::State_Selected) {
        painter->setPen(Utils::getContrastingColour(backgroundColour));
    }

    // Use QTextDocument for rich text
    QTextDocument *doc = new QTextDocument{};
    metrics = QFontMetrics{font};
    QString characters
        = metrics
              .elidedText(sentence.getCharacters(characterOptions).c_str(),
                          Qt::ElideRight,
                          r.width())
              .trimmed();
    doc->setHtml(characters);
    doc->setTextWidth(r.width());
    doc->setDefaultFont(font);
    doc->setDocumentMargin(0);
    QAbstractTextDocumentLayout *documentLayout = doc->documentLayout();
    auto ctx = QAbstractTextDocumentLayout::PaintContext();
    ctx.palette.setColor(QPalette::Text, painter->pen().color());
    QRectF bounds = QRectF(0, 0, r.width(), h4FontSize);
    ctx.clip = bounds;
    painter->translate(cellLeftPadding, r.y());
    documentLayout->draw(painter, ctx);
    painter->translate(-cellLeftPadding, -r.y());
    painter->restore();
    r = r.adjusted(0, h4FontSize + contentSpacingMargin, 0, 0);

    delete doc;

    // Phonetic and definition snippets
#ifdef Q_OS_WIN
    font = oldFont;
#endif
    QString snippet;
    font.setPixelSize(bodyFontSize);
    painter->setFont(font);
    metrics = QFontMetrics{font};
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
    if (phonetic.isEmpty()) {
        phonetic = "-";
    }
    painter->drawText(r, 0, phonetic, &boundingRect);
    painter->restore();
    r = r.adjusted(0, bodyFontSize + contentSpacingMargin, 0, 0);

    painter->save();
    if (option.state & QStyle::State_Selected) {
        painter->setPen(Utils::getContrastingColour(backgroundColour));
    }
#ifdef Q_OS_WIN
    oldFont = font;
    QString snippetLanguage = QString{sentence.getSentenceSnippetLanguage()
            .c_str()}.trimmed();
    if (snippetLanguage == "cmn" || snippetLanguage == "yue") {
        font = QFont{"Microsoft Yahei"};
    } else {
        font = oldFont;
    }
#endif
    font.setPixelSize(bodyFontSize);
    painter->setFont(font);
    snippet = metrics
                  .elidedText(sentence.getSentenceSnippet().c_str(),
                              Qt::ElideRight,
                              r.width())
                  .trimmed();
    painter->drawText(r, 0, snippet, &boundingRect);
    painter->restore();

    // Bottom divider
    QRect rct = option.rect;
    rct.setY(rct.bottom());
    painter->fillRect(rct, option.palette.alternateBase());

#ifdef Q_OS_WIN
    font = oldFont;
#endif
    painter->restore();
}

QSize SentenceResultListDelegate::sizeHint(const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    (void) (option);
    (void) (index);

    Settings::InterfaceSize interfaceSize
        = _settings
              ->value("Interface/size",
                      QVariant::fromValue(Settings::InterfaceSize::NORMAL))
              .value<Settings::InterfaceSize>();
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    switch (interfaceSize) {
    case Settings::InterfaceSize::SMALLER: {
        return QSize(100, 75);
    }
    case Settings::InterfaceSize::SMALL: {
        return QSize(100, 80);
    }
    case Settings::InterfaceSize::NORMAL: {
        return QSize(100, 90);
    }
    case Settings::InterfaceSize::LARGE: {
        return QSize(100, 100);
    }
    case Settings::InterfaceSize::LARGER: {
        return QSize(100, 110);
    }
    }
#else
    switch (interfaceSize) {
    case Settings::InterfaceSize::SMALLER: {
        return QSize(100, 88);
    }
    case Settings::InterfaceSize::SMALL: {
        return QSize(100, 95);
    }
    case Settings::InterfaceSize::NORMAL: {
        return QSize(100, 115);
    }
    case Settings::InterfaceSize::LARGE: {
        return QSize(100, 130);
    }
    case Settings::InterfaceSize::LARGER: {
        return QSize(100, 150);
    }
    }
#endif
}

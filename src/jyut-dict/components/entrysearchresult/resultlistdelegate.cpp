#include "resultlistdelegate.h"

#include "logic/entry/entry.h"
#include "logic/entry/entrycharactersoptions.h"
#include "logic/entry/entryphoneticoptions.h"
#include "logic/settings/settingsutils.h"
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

    EntryCharactersOptions characterOptions;
    EntryPhoneticOptions phoneticOptions;
    CantoneseOptions cantoneseOptions = CantoneseOptions::RAW_JYUTPING;
    MandarinOptions mandarinOptions = MandarinOptions::PRETTY_PINYIN;
    bool use_colours = false;
    if (isWelcomeEntry || isEmptyEntry) {
        characterOptions = EntryCharactersOptions::ONLY_SIMPLIFIED;
        phoneticOptions = EntryPhoneticOptions::ONLY_MANDARIN;
    } else {
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
        cantoneseOptions
            = Settings::getSettings()
                  ->value("Preview/cantonesePronunciationOptions",
                          QVariant::fromValue(CantoneseOptions::RAW_JYUTPING))
                  .value<CantoneseOptions>();
        mandarinOptions
            = Settings::getSettings()
                  ->value("Preview/mandarinPronunciationOptions",
                          QVariant::fromValue(MandarinOptions::PRETTY_PINYIN))
                  .value<MandarinOptions>();
        use_colours = !(option.state & QStyle::State_Selected);
    }
    entry.generatePhonetic(cantoneseOptions, mandarinOptions);

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
    int cellTopPadding = bodyFontSize;
    int cellLeftPadding = bodyFontSize;
    int contentSpacingMargin = bodyFontSize / 2;

    // Chinese characters
#ifdef Q_OS_WIN
    QFont oldFont = font;
    font = QFont("Microsoft Yahei");
#endif
    font.setPixelSize(h4FontSize);
    painter->setFont(font);
    r = option.rect.adjusted(cellLeftPadding,
                             cellTopPadding,
                             -cellLeftPadding,
                             0);

    // Use QTextDocument for rich text
    QTextDocument *doc = new QTextDocument{};
    entry.refreshColours(
        _settings
            ->value("entryColourPhoneticType",
                    QVariant::fromValue(EntryColourPhoneticType::CANTONESE))
            .value<EntryColourPhoneticType>());
    // Can't elide this text because QFontMetrics tries to elide the rich text
    // HTML annotations.
    QString characters
        = entry.getCharacters(characterOptions, use_colours).c_str();
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
    r = r.adjusted(0, h4FontSize + contentSpacingMargin * 2, 0, 0);

    delete doc;

    // Phonetic and definition snippets
#ifdef Q_OS_WIN
    font = oldFont;
#endif
    QFontMetrics metrics{font};
    QString snippet;
    if (isEmptyEntry) {
        r = r.adjusted(0, -contentSpacingMargin, 0, 0);
        font.setPixelSize(bodyFontSize + 2);
        painter->setFont(font);
        metrics = QFontMetrics(font);
        QString phonetic = metrics.elidedText(entry.getJyutping().c_str(),
                                              Qt::ElideRight,
                                              r.width());
        painter->drawText(r, 0, phonetic, &boundingRect);
        r = r.adjusted(0, bodyFontSize + 2 + contentSpacingMargin * 2, 0, 0);

        if (Settings::isCurrentLocaleHan()) {
            font.setPixelSize(bodyFontSizeHan);
        } else {
            font.setPixelSize(bodyFontSize);
        }
        painter->setFont(font);
        painter->save();
        painter->setPen(QPen(option.palette.color(QPalette::PlaceholderText)));

        // Do custom text layout to get eliding double-line label
        snippet = entry.getDefinitionSnippet().c_str();
        QTextLayout *textLayout = new QTextLayout{snippet, painter->font()};
        textLayout->beginLayout();

        // Define start and end y coordinates
        // max height of label is five lines, so height * 5
        int y = r.y();
        int height = y + metrics.height() * 5;

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
        font.setPixelSize(bodyFontSize);
        painter->setFont(font);
        // We can't get a bounding rect from QAbstractTextDocumentLayout::draw(),
        // so we have to manually adjust the location where the painter draws
        // phonetic + definition snippets
        metrics = QFontMetrics{font};
        QString phonetic = metrics.elidedText(entry
                                                  .getPhonetic(phoneticOptions,
                                                               cantoneseOptions,
                                                               mandarinOptions)
                                                  .c_str(),
                                              Qt::ElideRight,
                                              r.width());
        if (phonetic.isEmpty()) {
            phonetic = "-";
        }
        painter->drawText(r, 0, phonetic, &boundingRect);
        r = r.adjusted(0, bodyFontSize + contentSpacingMargin / 2, 0, 0);

        snippet = metrics.elidedText(
            entry.getDefinitionSnippet().c_str(),
            Qt::ElideRight, r.width());
        painter->drawText(r, 0, snippet, &boundingRect);
    }

    // Bottom divider
    QRect rct = option.rect;
    rct.setY(rct.bottom());
    painter->fillRect(rct, option.palette.alternateBase());

    painter->restore();
}

QSize ResultListDelegate::sizeHint(const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    (void) (option);

    Entry entry = qvariant_cast<Entry>(index.data());
    bool isEmptyEntry = entry.isEmpty();

    Settings::InterfaceSize interfaceSize
        = _settings
              ->value("Interface/size",
                      QVariant::fromValue(Settings::InterfaceSize::NORMAL))
              .value<Settings::InterfaceSize>();

    if (isEmptyEntry) {
#ifdef Q_OS_MAC
        switch (interfaceSize) {
        case Settings::InterfaceSize::SMALLER: {
            return QSize(100, 105);
        }
        case Settings::InterfaceSize::SMALL: {
            switch (Settings::getCurrentLocale().language()) {
            case QLocale::French: {
                return QSize(100, 130);
            }
            default: {
                return QSize(100, 115);
            }
            }
        }
        case Settings::InterfaceSize::NORMAL: {
            switch (Settings::getCurrentLocale().language()) {
            case QLocale::French: {
                return QSize(100, 150);
            }
            case QLocale::Chinese: {
                return QSize(100, 150);
            }
            case QLocale::Cantonese: {
                return QSize(100, 150);
            }
            default: {
                return QSize(100, 130);
            }
            }
        }
        case Settings::InterfaceSize::LARGE: {
            switch (Settings::getCurrentLocale().language()) {
            case QLocale::Chinese: {
                return QSize(100, 170);
            }
            case QLocale::French: {
                return QSize(100, 190);
            }
            default: {
                return QSize(100, 165);
            }
            }
        }
        case Settings::InterfaceSize::LARGER: {
            switch (Settings::getCurrentLocale().language()) {
            case QLocale::Chinese: {
                return QSize(100, 215);
            }
            case QLocale::Cantonese:
            case QLocale::French: {
                return QSize(100, 215);
            }
            default: {
                return QSize(100, 190);
            }
            }
        }
        }
#elif defined(Q_OS_LINUX)
        switch (interfaceSize) {
        case Settings::InterfaceSize::SMALLER: {
            return QSize(100, 110);
        }
        case Settings::InterfaceSize::SMALL: {
            switch (Settings::getCurrentLocale().language()) {
            case QLocale::Cantonese:
            case QLocale::French: {
                return QSize(100, 120);
            }
            default: {
                return QSize(100, 115);
            }
            }
        }
        case Settings::InterfaceSize::NORMAL: {
            return QSize(100, 150);
        }
        case Settings::InterfaceSize::LARGE: {
            switch (Settings::getCurrentLocale().language()) {
            case QLocale::Chinese:
            case QLocale::Cantonese: {
                return QSize(100, 180);
            }
            default: {
                return QSize(100, 190);
            }
            }
        }
        case Settings::InterfaceSize::LARGER: {
            return QSize(100, 215);
        }
        }
#elif defined(Q_OS_WIN)
        switch (interfaceSize) {
        case Settings::InterfaceSize::SMALLER: {
            return QSize(100, 115);
        }
        case Settings::InterfaceSize::SMALL: {
            return QSize(100, 125);
        }
        case Settings::InterfaceSize::NORMAL: {
            switch (Settings::getCurrentLocale().language()) {
            case QLocale::English: {
                return QSize(100, 150);
            }
            default: {
                return QSize(100, 165);
            }
            }
        }
        case Settings::InterfaceSize::LARGE: {
            switch (Settings::getCurrentLocale().language()) {
            case QLocale::French: {
                return QSize(100, 210);
            }
            default: {
                return QSize(100, 190);
            }
            }
        }
        case Settings::InterfaceSize::LARGER: {
            return QSize(100, 240);
        }
        }
#endif
    } else {
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
        switch (interfaceSize) {
        case Settings::InterfaceSize::SMALLER: {
            return QSize(100, 77);
        }
        case Settings::InterfaceSize::SMALL: {
            return QSize(100, 82);
        }
        case Settings::InterfaceSize::NORMAL: {
            return QSize(100, 97);
        }
        case Settings::InterfaceSize::LARGE: {
            return QSize(100, 112);
        }
        case Settings::InterfaceSize::LARGER: {
            return QSize(100, 127);
        }
        }
#else
        switch (interfaceSize) {
        case Settings::InterfaceSize::SMALLER: {
            return QSize(100, 72);
        }
        case Settings::InterfaceSize::SMALL: {
            return QSize(100, 78);
        }
        case Settings::InterfaceSize::NORMAL: {
            return QSize(100, 90);
        }
        case Settings::InterfaceSize::LARGE: {
            return QSize(100, 105);
        }
        case Settings::InterfaceSize::LARGER: {
            return QSize(100, 120);
        }
        }
#endif
    }

    // All cases should be handled and the function should
    // never reach here.
    return QSize(100, 100);
}

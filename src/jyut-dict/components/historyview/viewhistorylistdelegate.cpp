#include "viewhistorylistdelegate.h"

#include "logic/entry/entry.h"
#include "logic/entry/entrycharactersoptions.h"
#include "logic/entry/entryphoneticoptions.h"
#include "logic/settings/settings.h"
#include "logic/settings/settingsutils.h"
#include "logic/utils/utils_qt.h"

#include <QGuiApplication>
#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QTextLayout>
#include <QVariant>

ViewHistoryListDelegate::ViewHistoryListDelegate(QWidget *parent)
    : QStyledItemDelegate (parent)
{
    _settings = Settings::getSettings(this);
}

void ViewHistoryListDelegate::paint(QPainter *painter,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index) const
{
    if (!index.data().canConvert<Entry>()) {
        return;
    }

    painter->save();

    Entry entry = qvariant_cast<Entry>(index.data());

    bool isEmptyEntry = entry.isEmpty();

    QColor backgroundColour;
    if (option.state & QStyle::State_Selected && !isEmptyEntry) {
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
    if (isEmptyEntry) {
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
        entry.generatePhonetic(cantoneseOptions, mandarinOptions);

        use_colours = !(option.state & QStyle::State_Selected);
    }

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

    // Chinese characters
#ifdef Q_OS_WIN
    QFont oldFont = font;
    font = QFont("Microsoft Yahei");
#endif
    font.setPixelSize(h4FontSize);
    painter->setFont(font);
    r = option.rect.adjusted(cellTopPadding,
                             cellLeftPadding,
                             -cellLeftPadding,
                             0);

    QTextDocument *doc = new QTextDocument{};
    entry.refreshColours(_settings
                             ->value("entryColourPhoneticType",
                                     QVariant::fromValue(
                                         EntryColourPhoneticType::CANTONESE))
                             .value<EntryColourPhoneticType>());
    doc->setHtml(QString(entry.getCharacters(characterOptions, use_colours).c_str()));
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

    // Phonetic
#ifdef Q_OS_WIN
    font = oldFont;
#endif
    QFontMetrics metrics{font};
    QString snippet;
    if (isEmptyEntry) {
        font.setPixelSize(bodyFontSize + 2);
        painter->setFont(font);
        metrics = QFontMetrics(font);
        QString phonetic = metrics.elidedText(entry.getJyutping().c_str(),
                                              Qt::ElideRight,
                                              r.width());
        painter->drawText(r, 0, phonetic, &boundingRect);
        r = r.adjusted(0, bodyFontSize + 2 + contentSpacingMargin, 0, 0);

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
        painter->restore();
    } else {
        font.setPixelSize(bodyFontSize);
        painter->setFont(font);
        metrics = QFontMetrics(font);
        QString phonetic = metrics.elidedText(entry
                                                  .getPhonetic(phoneticOptions,
                                                               cantoneseOptions,
                                                               mandarinOptions)
                                                  .c_str(),
                                              Qt::ElideRight,
                                              r.width());
        painter->drawText(r, 0, phonetic, &boundingRect);
    }

    // Bottom divider
    QRect rct = option.rect;
    rct.setY(rct.bottom() - 1);
    painter->fillRect(rct, option.palette.alternateBase());

    painter->restore();
}

QSize ViewHistoryListDelegate::sizeHint(const QStyleOptionViewItem &option,
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
            return QSize(100, 90);
        }
        case Settings::InterfaceSize::SMALL: {
            return QSize(100, 100);
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
            case QLocale::Cantonese: {
                return QSize(100,
                             Settings::getCurrentLocale().script()
                                     == QLocale::SimplifiedHanScript
                                 ? 100
                                 : 120);
            }
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
            return QSize(100, 200);
        }
        }
#endif
    } else {
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
        switch (interfaceSize) {
        case Settings::InterfaceSize::SMALLER: {
            return QSize(100, 60);
        }
        case Settings::InterfaceSize::SMALL: {
            return QSize(100, 65);
        }
        case Settings::InterfaceSize::NORMAL: {
            return QSize(100, 70);
        }
        case Settings::InterfaceSize::LARGE: {
            return QSize(100, 85);
        }
        case Settings::InterfaceSize::LARGER: {
            return QSize(100, 95);
        }
        }
#else
        switch (interfaceSize) {
        case Settings::InterfaceSize::SMALLER: {
            return QSize(100, 55);
        }
        case Settings::InterfaceSize::SMALL: {
            return QSize(100, 60);
        }
        case Settings::InterfaceSize::NORMAL: {
            return QSize(100, 70);
        }
        case Settings::InterfaceSize::LARGE: {
            return QSize(100, 80);
        }
        case Settings::InterfaceSize::LARGER: {
            return QSize(100, 90);
        }
        }
#endif
    }
}

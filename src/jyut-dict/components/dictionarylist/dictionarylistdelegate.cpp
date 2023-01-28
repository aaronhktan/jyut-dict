#include "dictionarylistdelegate.h"

#include "logic/dictionary/dictionarymetadata.h"
#include "logic/settings/settings.h"
#include "logic/settings/settingsutils.h"
#include "logic/utils/utils.h"
#include "logic/utils/utils_qt.h"

#include <QGuiApplication>

DictionaryListDelegate::DictionaryListDelegate(QWidget *parent)
    : QStyledItemDelegate(parent)
{
    _settings = Settings::getSettings(this);
}

void DictionaryListDelegate::paint(QPainter *painter,
                                   const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    if (!index.data().canConvert<DictionaryMetadata>()) {
        return;
    }

    painter->save();

    DictionaryMetadata source = qvariant_cast<DictionaryMetadata>(index.data());

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
        QColor textColour{Utils::getContrastingColour(backgroundColour)};
        painter->setPen(textColour);
    } else {
        painter->fillRect(option.rect, option.palette.base());
        painter->setPen(QPen{option.palette.color(QPalette::WindowText)});
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
    int bodyFontSize = Settings::bodyFontSize.at(
        static_cast<unsigned long>(interfaceSize - 1));
    int cellTopPadding = bodyFontSize * 2 / 3;
    int cellLeftPadding = bodyFontSize * 2 / 3;

    r = r.adjusted(cellTopPadding,
                   cellLeftPadding,
                   -cellTopPadding,
                   -cellLeftPadding);
    font.setPixelSize(bodyFontSize);
    painter->setFont(font);
    QFontMetrics metrics{font};
    QString sourcename = metrics.elidedText(
        source.getName().c_str(),
        Qt::ElideRight,
        r.width());
    painter->drawText(r, 0, sourcename, &boundingRect);

    painter->restore();
}

QSize DictionaryListDelegate::sizeHint(const QStyleOptionViewItem &option,
                                       const QModelIndex &index) const
{
    (void) (option);
    (void) (index);

    Settings::InterfaceSize interfaceSize
        = _settings
              ->value("Interface/size",
                      QVariant::fromValue(Settings::InterfaceSize::NORMAL))
              .value<Settings::InterfaceSize>();

#ifdef Q_OS_MAC
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
#else
    switch (interfaceSize) {
    case Settings::InterfaceSize::SMALLER: {
        return QSize(100, 28);
    }
    case Settings::InterfaceSize::SMALL: {
        return QSize(100, 33);
    }
    case Settings::InterfaceSize::NORMAL: {
        return QSize(100, 38);
    }
    case Settings::InterfaceSize::LARGE: {
        return QSize(100, 43);
    }
    case Settings::InterfaceSize::LARGER: {
        return QSize(100, 48);
    }
    }
#endif
}

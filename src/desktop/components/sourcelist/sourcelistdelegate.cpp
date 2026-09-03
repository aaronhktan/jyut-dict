#include "sourcelistdelegate.h"

#include "logic/source/sourcemetadata.h"
#include "logic/settings/settings.h"
#include "logic/settings/settingsutils.h"
#include "logic/utils/utils_qt.h"

#include <QGuiApplication>
#include <QModelIndex>
#include <QPainter>
#include <QStyleOptionViewItem>

SourceListDelegate::SourceListDelegate(QWidget *parent)
    : QStyledItemDelegate{parent}
    , _settings{Settings::getSettings(this)}
{
}

void SourceListDelegate::paint(QPainter *painter,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index) const
{
    if (!index.data().canConvert<SourceMetadata>()) {
        return;
    }

    painter->save();

    SourceMetadata source{qvariant_cast<SourceMetadata>(index.data())};

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

    QRect r{option.rect};
    QRect boundingRect;
    QFont font{painter->font()};
    const int interfaceSize = static_cast<int>(
        _settings
            ->value("Interface/size",
                    QVariant::fromValue(Settings::InterfaceSize::NORMAL))
            .value<Settings::InterfaceSize>());
    const int bodyFontSize = Settings::bodyFontSize.at(
        static_cast<unsigned long>(interfaceSize - 1));
    const int cellTopPadding = bodyFontSize * 2 / 3;
    const int cellLeftPadding = bodyFontSize * 2 / 3;

    r = r.adjusted(cellTopPadding,
                   cellLeftPadding,
                   -cellTopPadding,
                   -cellLeftPadding);
    font.setPixelSize(bodyFontSize);
    painter->setFont(font);
    const QFontMetrics metrics{font};
    const QString sourcename = metrics.elidedText(source.getName().c_str(),
                                                  Qt::ElideRight,
                                                  r.width());
    painter->drawText(r, 0, sourcename, &boundingRect);

    painter->restore();
}

QSize SourceListDelegate::sizeHint(
    [[maybe_unused]] const QStyleOptionViewItem &option,
    [[maybe_unused]] const QModelIndex &index) const
{
    Settings::InterfaceSize interfaceSize
        = _settings
              ->value("Interface/size",
                      QVariant::fromValue(Settings::InterfaceSize::NORMAL))
              .value<Settings::InterfaceSize>();

#ifdef Q_OS_MAC
    switch (interfaceSize) {
    case Settings::InterfaceSize::SMALLER: {
        return QSize{100, 25};
    }
    case Settings::InterfaceSize::SMALL: {
        return QSize{100, 30};
    }
    case Settings::InterfaceSize::NORMAL: {
        return QSize{100, 35};
    }
    case Settings::InterfaceSize::LARGE: {
        return QSize{100, 40};
    }
    case Settings::InterfaceSize::LARGER: {
        return QSize{100, 45};
    }
    }
#else
    switch (interfaceSize) {
    case Settings::InterfaceSize::SMALLER: {
        return QSize{100, 28};
    }
    case Settings::InterfaceSize::SMALL: {
        return QSize{100, 33};
    }
    case Settings::InterfaceSize::NORMAL: {
        return QSize{100, 38};
    }
    case Settings::InterfaceSize::LARGE: {
        return QSize{100, 43};
    }
    case Settings::InterfaceSize::LARGER: {
        return QSize{100, 48};
    }
    }
#endif

    // All cases should be handled and the function should
    // never reach here.
    return QSize{100, 100};
}

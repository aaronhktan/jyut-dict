#include "magnifyscrollareawidget.h"

#include "logic/settings/settings.h"
#include "logic/settings/settingsutils.h"
#include "logic/utils/utils_qt.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif

#include <QEvent>
#include <QTimer>

MagnifyScrollAreaWidget::MagnifyScrollAreaWidget(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("MagnifyScrollAreaWidget");

    _settings = Settings::getSettings(this);

    _scrollAreaLayout = new QVBoxLayout{this};
    _scrollAreaLayout->setSpacing(0);
    _scrollAreaLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(_scrollAreaLayout);

    // For some really weird reason, putting the labels inside another widget
    // helps with word wrapping issues. I'm not sure exactly why.
    _widget = new QWidget{this};
    _widgetLayout = new QVBoxLayout{_widget};
    _widgetLayout->setSpacing(0);
    _widgetLayout->setContentsMargins(11, 11, 11, 11);
    _scrollAreaLayout->addWidget(_widget);

    _traditionalLabelLabel = new QLabel{_widget};
    _traditionalLabelLabel->setAlignment(Qt::AlignCenter);
    _traditionalLabel = new QLabel{_widget};
    _traditionalLabel->setAlignment(Qt::AlignCenter);
    _traditionalLabel->setWordWrap(true);
    _simplifiedLabelLabel = new QLabel{_widget};
    _simplifiedLabelLabel->setAlignment(Qt::AlignCenter);
    _simplifiedLabel = new QLabel{_widget};
    _simplifiedLabel->setAlignment(Qt::AlignCenter);
    _simplifiedLabel->setWordWrap(true);

    translateUI();
}

void MagnifyScrollAreaWidget::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, this, [=]() { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QWidget::changeEvent(event);
}

void MagnifyScrollAreaWidget::setEntry(const Entry &entry)
{
    EntryCharactersOptions characterOptions
        = _settings
              ->value("characterOptions",
                      QVariant::fromValue(
                          EntryCharactersOptions::PREFER_TRADITIONAL))
              .value<EntryCharactersOptions>();

    _traditionalLabel->setText(
        entry
            .getCharactersNoSecondary(EntryCharactersOptions::ONLY_TRADITIONAL,
                                      /* use_colours */ true)
            .c_str());
    _simplifiedLabel->setText(
        entry
            .getCharactersNoSecondary(EntryCharactersOptions::ONLY_SIMPLIFIED,
                                      /* use_colours */ true)
            .c_str());

    switch (characterOptions) {
    case EntryCharactersOptions::ONLY_SIMPLIFIED:
        _traditionalLabelLabel->setVisible(false);
        _traditionalLabel->setVisible(false);
    case EntryCharactersOptions::PREFER_SIMPLIFIED:
        if (_simplifiedLabel->text().isEmpty()) {
            _simplifiedLabelLabel->setVisible(false);
            _simplifiedLabel->setVisible(false);
            break;
        }
        _simplifiedLabelLabel->setVisible(true);
        _simplifiedLabel->setVisible(true);
        _widgetLayout->addWidget(_simplifiedLabelLabel, Qt::AlignHCenter);
        _widgetLayout->addWidget(_simplifiedLabel);
        break;
    case EntryCharactersOptions::ONLY_TRADITIONAL:
        _simplifiedLabelLabel->setVisible(false);
        _simplifiedLabel->setVisible(false);
    case EntryCharactersOptions::PREFER_TRADITIONAL:
        if (_traditionalLabel->text().isEmpty()) {
            _traditionalLabelLabel->setVisible(false);
            _traditionalLabel->setVisible(false);
            break;
        }
        _traditionalLabelLabel->setVisible(true);
        _traditionalLabel->setVisible(true);
        _widgetLayout->addWidget(_traditionalLabelLabel, Qt::AlignHCenter);
        _widgetLayout->addWidget(_traditionalLabel);
        break;
    }

    switch (characterOptions) {
    case EntryCharactersOptions::PREFER_SIMPLIFIED:
        if (_traditionalLabel->text().isEmpty()) {
            _traditionalLabelLabel->setVisible(false);
            _traditionalLabel->setVisible(false);
            break;
        }
        _traditionalLabelLabel->setVisible(true);
        _traditionalLabel->setVisible(true);
        _widgetLayout->addWidget(_traditionalLabelLabel, Qt::AlignHCenter);
        _widgetLayout->addWidget(_traditionalLabel);
        break;
    case EntryCharactersOptions::PREFER_TRADITIONAL:
        if (_simplifiedLabel->text().isEmpty()) {
            _simplifiedLabelLabel->setVisible(false);
            _simplifiedLabel->setVisible(false);
            break;
        }
        _simplifiedLabelLabel->setVisible(true);
        _simplifiedLabel->setVisible(true);
        _widgetLayout->addWidget(_simplifiedLabelLabel, Qt::AlignHCenter);
        _widgetLayout->addWidget(_simplifiedLabel);
        break;
    default:
        break;
    }

    setStyle(Utils::isDarkMode());
}

void MagnifyScrollAreaWidget::translateUI(void)
{
    _traditionalLabelLabel->setText(tr("Traditional"));
    _simplifiedLabelLabel->setText(tr("Simplified"));
    _traditionalLabelLabel->setSizePolicy(QSizePolicy::Maximum,
                                          QSizePolicy::Maximum);
    _simplifiedLabelLabel->setSizePolicy(QSizePolicy::Maximum,
                                         QSizePolicy::Maximum);
}

void MagnifyScrollAreaWidget::setStyle(bool use_dark)
{
    (void) (use_dark);

#ifdef Q_OS_WIN
    QFont font = QFont{"Microsoft YaHei"};
    font.setStyleHint(QFont::System, QFont::PreferAntialias);
    _traditionalLabel->setFont(font);
    _simplifiedLabel->setFont(font);
#endif

    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet("QWidget#MagnifyScrollAreaWidget { "
                  "   background-color: palette(base); "
                  "} ");

    _traditionalLabel->setStyleSheet("QLabel { "
                                     "   font-size: 120px; "
                                     "}");
    _simplifiedLabel->setStyleSheet("QLabel { "
                                    "   font-size: 120px; "
                                    "}");

    QColor textColour = use_dark ? QColor{LABEL_TEXT_COLOUR_DARK_R,
                                          LABEL_TEXT_COLOUR_DARK_G,
                                          LABEL_TEXT_COLOUR_DARK_B}
                                 : QColor{LABEL_TEXT_COLOUR_LIGHT_R,
                                          LABEL_TEXT_COLOUR_LIGHT_G,
                                          LABEL_TEXT_COLOUR_LIGHT_B};
    QColor borderColour = use_dark ? textColour.darker(300)
                                   : textColour.lighter(200);
    int interfaceSize = static_cast<int>(
        _settings
            ->value("Interface/size",
                    QVariant::fromValue(Settings::InterfaceSize::NORMAL))
            .value<Settings::InterfaceSize>());
    int bodyFontSize = Settings::bodyFontSize.at(
        static_cast<unsigned long>(interfaceSize - 1));
    int borderRadius = static_cast<int>(bodyFontSize * 1);
    int padding = bodyFontSize / 6;
    int paddingHorizontal = bodyFontSize;
    QString styleSheet = "QLabel { "
                         "   border: 2px solid %1; "
                         "   border-radius: %2px; "
                         "   color: %3; "
                         "   font-size: %4px; "
                         "   icon-size: %4px; "
                         "   padding: %5px; "
                         "   padding-left: %6px; "
                         "   padding-right: %6px; "
                         "} ";
    _traditionalLabelLabel->setStyleSheet(styleSheet.arg(borderColour.name())
                                              .arg(borderRadius)
                                              .arg(textColour.name())
                                              .arg(bodyFontSize)
                                              .arg(padding)
                                              .arg(paddingHorizontal));
    _traditionalLabelLabel->setFixedHeight(borderRadius * 2);
    _traditionalLabelLabel->setFixedWidth(
        _traditionalLabelLabel->sizeHint().width());
    _simplifiedLabelLabel->setStyleSheet(styleSheet.arg(borderColour.name())
                                             .arg(borderRadius)
                                             .arg(textColour.name())
                                             .arg(bodyFontSize)
                                             .arg(padding)
                                             .arg(paddingHorizontal));
    _simplifiedLabelLabel->setFixedHeight(borderRadius * 2);
    _simplifiedLabelLabel->setFixedWidth(
        _simplifiedLabelLabel->sizeHint().width());
}

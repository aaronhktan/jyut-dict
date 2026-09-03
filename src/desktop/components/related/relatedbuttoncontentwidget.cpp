#include "relatedbuttoncontentwidget.h"

#include "logic/entry/entry.h"
#include "logic/settings/settings.h"
#include "logic/settings/settingsutils.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined(Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QEvent>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

RelatedButtonContentWidget::RelatedButtonContentWidget(RelatedType type,
                                                       QWidget *parent)
    : QWidget{parent}
    , _settings{Settings::getSettings(this)}
    , _buttonType{type}
{
    setObjectName("RelatedButtonContentWidget");

    setupUI();
    translateUI();
    setStyle(Utils::isDarkMode());
}

void RelatedButtonContentWidget::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, this, [this] { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
        setStyle(Utils::isDarkMode());
    }
    QWidget::changeEvent(event);
}

void RelatedButtonContentWidget::setEntry(const Entry &entry)
{
    _traditional = entry.getTraditional();
    _simplified = entry.getSimplified();
}

void RelatedButtonContentWidget::setupUI()
{
    _relatedButtonLayout = new QVBoxLayout{this};
    _relatedButtonLayout->setContentsMargins(11, 11, 11, 11);
    _relatedButtonLayout->setSpacing(11);
    _relatedButtonLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    _description = new QLabel{this};
    _description->setWordWrap(true);
    _description->setAlignment(Qt::AlignCenter);
    _actionButton = new QPushButton{this};

    _relatedButtonLayout->addWidget(_description);
    _relatedButtonLayout->addWidget(_actionButton);

    connect(_actionButton,
            &QPushButton::clicked,
            this,
            &RelatedButtonContentWidget::searchQueryRequested);
}

void RelatedButtonContentWidget::translateUI()
{
    switch (_buttonType) {
    case RelatedType::SearchBeginning: {
        _description->setText(tr("Find entries that begin with this entry"));
        break;
    }
    case RelatedType::SearchContaining: {
        _description->setText(tr("Find entries that contain this entry"));
        break;
    }
    case RelatedType::SearchEnding: {
        _description->setText(tr("Find entries that end with this entry"));
        break;
    }
    }
    _actionButton->setText(tr("Search →"));
}

void RelatedButtonContentWidget::setStyle(bool use_dark)
{
    const QColor textColour = use_dark
                                  ? QColor{Utils::LABEL_TEXT_COLOUR_DARK_R,
                                           Utils::LABEL_TEXT_COLOUR_DARK_G,
                                           Utils::LABEL_TEXT_COLOUR_DARK_B}
                                  : QColor{Utils::LABEL_TEXT_COLOUR_LIGHT_R,
                                           Utils::LABEL_TEXT_COLOUR_LIGHT_G,
                                           Utils::LABEL_TEXT_COLOUR_LIGHT_B};
    const QColor borderColour
        = use_dark ? QColor{Utils::CONTENT_BACKGROUND_COLOUR_DARK_R,
                            Utils::CONTENT_BACKGROUND_COLOUR_DARK_G,
                            Utils::CONTENT_BACKGROUND_COLOUR_DARK_B}
                   : QColor{Utils::CONTENT_BACKGROUND_COLOUR_LIGHT_R,
                            Utils::CONTENT_BACKGROUND_COLOUR_LIGHT_G,
                            Utils::CONTENT_BACKGROUND_COLOUR_LIGHT_B};
    const QColor backgroundColor
        = use_dark ? QColor{Utils::HEADER_BACKGROUND_COLOUR_DARK_R,
                            Utils::HEADER_BACKGROUND_COLOUR_DARK_G,
                            Utils::HEADER_BACKGROUND_COLOUR_DARK_B}
                   : QColor{Utils::CONTENT_BACKGROUND_COLOUR_LIGHT_R,
                            Utils::CONTENT_BACKGROUND_COLOUR_LIGHT_G,
                            Utils::CONTENT_BACKGROUND_COLOUR_LIGHT_B};
    const int interfaceSize = static_cast<int>(
        _settings
            ->value("Interface/size",
                    QVariant::fromValue(Settings::InterfaceSize::NORMAL))
            .value<Settings::InterfaceSize>());
    const int bodyFontSize = Settings::bodyFontSize.at(
        static_cast<unsigned long>(interfaceSize - 1));
    const int borderRadius = static_cast<int>(bodyFontSize * 1.5);
    const int padding = bodyFontSize / 6;
    const int paddingHorizontal = bodyFontSize / 3;

    const QString descriptionLabelStyleSheet = "QLabel { "
                                               "   font-size: %2px; "
                                               "}";
    _description->setStyleSheet(descriptionLabelStyleSheet.arg(bodyFontSize));

    QString buttonStyleSheet;
    if (use_dark) {
        buttonStyleSheet = "QPushButton { "
                           "   background-color: %7; "
#ifdef Q_OS_WIN
                           "   border: 1px solid %1; "
#else
                           "   border: 2px solid %1; "
#endif
                           "   border-radius: %2px; "
                           "   color: %3; "
                           "   font-size: %4px; "
                           "   icon-size: %4px; "
                           "   padding: %5px; "
                           "   padding-left: %6px; "
                           "   padding-right: %6px; "
                           "} "
                           ""
                           "QPushButton:hover { "
                           "   background-color: %8; "
#ifdef Q_OS_WIN
                           "   border: 1px solid %1; "
#else
                           "   border: 2px solid %1; "
#endif
                           "   border-radius: %2px; "
                           "   color: %3; "
                           "   font-size: %4px; "
                           "   icon-size: %4px; "
                           "   padding: %5px; "
                           "   padding-left: %6px; "
                           "   padding-right: %6px; "
                           "} ";
        buttonStyleSheet = buttonStyleSheet.arg(borderColour.name())
                               .arg(borderRadius)
                               .arg(textColour.name())
                               .arg(bodyFontSize)
                               .arg(padding)
                               .arg(paddingHorizontal)
                               .arg(backgroundColor.name(),
                                    backgroundColor.lighter(125).name());
    } else {
        buttonStyleSheet = "QPushButton { "
                           "   background-color: palette(base); "
#ifdef Q_OS_WIN
                           "   border: 1px solid %1; "
#else
                           "   border: 2px solid %1; "
#endif
                           "   border-radius: %2px; "
                           "   color: %3; "
                           "   font-size: %4px; "
                           "   icon-size: %4px; "
                           "   padding: %5px; "
                           "   padding-left: %6px; "
                           "   padding-right: %6px; "
                           "} "
                           ""
                           "QPushButton:hover { "
                           "   background-color: %1; "
#ifdef Q_OS_WIN
                           "   border: 1px solid %1; "
#else
                           "   border: 2px solid %1; "
#endif
                           "   border-radius: %2px; "
                           "   color: %3; "
                           "   font-size: %4px; "
                           "   icon-size: %4px; "
                           "   padding: %5px; "
                           "   padding-left: %6px; "
                           "   padding-right: %6px; "
                           "} ";
        buttonStyleSheet = buttonStyleSheet.arg(borderColour.name())
                               .arg(borderRadius)
                               .arg(textColour.name())
                               .arg(bodyFontSize)
                               .arg(padding)
                               .arg(paddingHorizontal);
    }

    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (const auto &button, buttons) {
        button->setStyleSheet(buttonStyleSheet);
        button->setMinimumHeight(borderRadius * 2);

#ifdef Q_OS_MAC
        // Hack to get around weird button sizing issues when switching styles
        button->setVisible(false);
        button->setVisible(true);
#endif
    }
}

void RelatedButtonContentWidget::searchRequested(void)
{
    if (_actionButton->isVisible()) {
        _actionButton->click();
    }
}

void RelatedButtonContentWidget::searchQueryRequested(void)
{
    SearchParameters params;
    QString queryCharacters;

    EntryCharactersOptions characterOptions
        = _settings
              ->value("characterOptions",
                      QVariant::fromValue(
                          EntryCharactersOptions::PREFER_TRADITIONAL))
              .value<EntryCharactersOptions>();
    switch (characterOptions) {
    case EntryCharactersOptions::PREFER_TRADITIONAL:
    case EntryCharactersOptions::ONLY_TRADITIONAL: {
        params = SearchParameters::TRADITIONAL;
        queryCharacters = _traditional.c_str();
        break;
    }
    case EntryCharactersOptions::PREFER_SIMPLIFIED:
    case EntryCharactersOptions::ONLY_SIMPLIFIED: {
        params = SearchParameters::SIMPLIFIED;
        queryCharacters = _simplified.c_str();
        break;
    }
    }

    QString query;

    switch (_buttonType) {
    case RelatedType::SearchBeginning: {
        query = queryCharacters + "?*";
        break;
    }
    case RelatedType::SearchContaining: {
        query = "*?" + queryCharacters + "?*";
        break;
    }
    case RelatedType::SearchEnding: {
        query = "*?" + queryCharacters + "$";
        break;
    }
    }

    emit searchQuery(query, params);
}

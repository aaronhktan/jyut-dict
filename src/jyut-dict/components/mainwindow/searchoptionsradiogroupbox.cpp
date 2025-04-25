#include "searchoptionsradiogroupbox.h"

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

#include <QLocale>
#include <QStyle>
#include <QTimer>

SearchOptionsRadioGroupBox::SearchOptionsRadioGroupBox(
    std::shared_ptr<ISearchOptionsMediator> mediator,
    std::shared_ptr<SQLSearch> manager,
    QWidget *parent)
    : QGroupBox(parent)
{
    _mediator = mediator;
    _search = manager;
    _settings = Settings::getSettings(this);

    _search->registerObserver(this);

    setupUI();
    translateUI();

    SearchParameters lastSelected
        = _settings
              ->value("SearchOptionsRadioGroupBox/lastSelected",
                      QVariant::fromValue(SearchParameters::ENGLISH))
              .value<SearchParameters>();
    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (const auto & button, buttons) {
        if (button->property("data").value<SearchParameters>() == lastSelected) {
            button->click();
            break;
        }
    }
}

void SearchOptionsRadioGroupBox::detectedLanguage(SearchParameters params)
{
    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (const auto &button, buttons) {
        if (button->property("data") == QVariant::fromValue(params)) {
            button->setChecked(true);
            Settings::getSettings()
                ->setValue("SearchOptionsRadioGroupBox/lastSelected",
                           button->property("data"));
        } else {
            button->setChecked(false);
        }
    }
}

void SearchOptionsRadioGroupBox::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, this, [=, this]() { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QGroupBox::changeEvent(event);
}

void SearchOptionsRadioGroupBox::setOption(const Utils::ButtonOptionIndex index)
{
    if (index >= Utils::MAXIMUM_BUTTON_INDEX) {
        return;
    }

    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    buttons.at(index)->click();
}

void SearchOptionsRadioGroupBox::setOption(const SearchParameters parameters)
{
    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (const auto & button, buttons) {
        if (button->property("data") == QVariant::fromValue(parameters)) {
            button->click();
        }
    }
}

void SearchOptionsRadioGroupBox::setupUI()
{
    setObjectName("SearchOptionsRadioGroupBox");
    setContentsMargins(0, 0, 0, 0);
    setAttribute(Qt::WA_LayoutOnEntireRect);

    _layout = new QHBoxLayout{this};
    _layout->setContentsMargins(0, 0, 0, 0);
#ifdef Q_OS_LINUX
    _layout->setSpacing(5);
#else
    _layout->setSpacing(10);
#endif

    _currentChoiceLabel = new QLabel{this};
    _simplifiedButton = new QPushButton{this};
    _traditionalButton = new QPushButton{this};
    _jyutpingButton = new QPushButton{this};
    _pinyinButton = new QPushButton{this};
    _englishButton = new QPushButton{this};

    _currentChoiceLabel->setIndent(0);
    _simplifiedButton->setCheckable(true);
    _traditionalButton->setCheckable(true);
    _jyutpingButton->setCheckable(true);
    _pinyinButton->setCheckable(true);
    _englishButton->setCheckable(true);

    _simplifiedButton->setProperty("data",
                                   QVariant::fromValue(
                                       SearchParameters::SIMPLIFIED));
    _traditionalButton->setProperty("data",
                                    QVariant::fromValue(
                                        SearchParameters::TRADITIONAL));
    _jyutpingButton->setProperty("data",
                                 QVariant::fromValue(
                                     SearchParameters::JYUTPING));
    _pinyinButton->setProperty("data",
                               QVariant::fromValue(SearchParameters::PINYIN));
    _englishButton->setProperty("data",
                                QVariant::fromValue(SearchParameters::ENGLISH));

    connect(_simplifiedButton,
            &QPushButton::clicked,
            this,
            &SearchOptionsRadioGroupBox::notifyMediator);
    connect(_traditionalButton,
            &QPushButton::clicked,
            this,
            &SearchOptionsRadioGroupBox::notifyMediator);
    connect(_jyutpingButton,
            &QPushButton::clicked,
            this,
            &SearchOptionsRadioGroupBox::notifyMediator);
    connect(_pinyinButton,
            &QPushButton::clicked,
            this,
            &SearchOptionsRadioGroupBox::notifyMediator);
    connect(_englishButton,
            &QPushButton::clicked,
            this,
            &SearchOptionsRadioGroupBox::notifyMediator);

#ifdef Q_OS_MAC
    _layout->addWidget(_currentChoiceLabel, 0, Qt::AlignLeft | Qt::AlignBottom);
#else
    _layout->addWidget(_currentChoiceLabel, 0);
#endif
    _layout->addWidget(_simplifiedButton);
    _layout->addWidget(_traditionalButton);
    _layout->addWidget(_jyutpingButton);
    _layout->addWidget(_pinyinButton);
    _layout->addWidget(_englishButton);
    _layout->addStretch(1);

    setFlat(true);
    setStyle(Utils::isDarkMode());
}

void SearchOptionsRadioGroupBox::translateUI()
{
    // Set property so styling automatically changes
    setProperty("isHan", Settings::isCurrentLocaleHan());

    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (const auto & button, buttons) {
        button->setProperty("isHan", Settings::isCurrentLocaleHan());
        button->style()->unpolish(button);
        button->style()->polish(button);
    }

    _currentChoiceLabel->setText(tr("Searching:"));
    _simplifiedButton->setText(tr("Simplified Chinese"));
    _traditionalButton->setText(tr("Traditional Chinese"));
    _jyutpingButton->setText(tr("Jyutping"));
    _pinyinButton->setText(tr("Pinyin"));
    _englishButton->setText(tr("English"));

    _simplifiedButton->setToolTip(tr("Search Simplified Chinese"));
    _traditionalButton->setToolTip(tr("Search Traditional Chinese"));
    _jyutpingButton->setToolTip(tr("Search Jyutping"));
    _pinyinButton->setToolTip(tr("Search Pinyin"));
    _englishButton->setToolTip(tr("Search English"));
}

void SearchOptionsRadioGroupBox::setStyle(bool use_dark)
{
    QColor borderColour = use_dark ? QColor{HEADER_BACKGROUND_COLOUR_DARK_R,
                                            HEADER_BACKGROUND_COLOUR_DARK_G,
                                            HEADER_BACKGROUND_COLOUR_DARK_B}
                                   : QColor{CONTENT_BACKGROUND_COLOUR_LIGHT_R,
                                            CONTENT_BACKGROUND_COLOUR_LIGHT_G,
                                            CONTENT_BACKGROUND_COLOUR_LIGHT_B};
#ifdef Q_OS_LINUX
    borderColour = borderColour.lighter(200);
#endif
    int interfaceSize = static_cast<int>(
        _settings
            ->value("Interface/size",
                    QVariant::fromValue(Settings::InterfaceSize::NORMAL))
            .value<Settings::InterfaceSize>());
    int bodyFontSize = Settings::bodyFontSize.at(
        static_cast<unsigned long>(interfaceSize - 1));
    int borderRadius = static_cast<int>(bodyFontSize * 1);
#ifdef Q_OS_MAC
    int padding = bodyFontSize / 3;
#else
    int padding = bodyFontSize / 6;
#endif
    int paddingHorizontal = bodyFontSize;

#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    setStyleSheet("QGroupBox#SearchOptionsRadioGroupBox { "
                  "   border: 0; "
                  "} ");
#endif
    QString styleSheet = "QPushButton { "
                         "   background-color: transparent; "
                         "   border: 2px solid %1; "
                         "   border-radius: %2px; "
                         "   font-size: %3px; "
                         "   padding: %4px; "
                         "   padding-left: %5px; "
                         "   padding-right: %5px; "
                         "} "
                         " "
                         "QPushButton:checked { "
                         "   background-color: %1; "
                         "   border: 2px solid %1; "
                         "   border-radius: %2px; "
                         "   font-size: %3px; "
                         "   padding: %4px; "
                         "   padding-left: %5px; "
                         "   padding-right: %5px; "
                         "} "
                         " "
                         "QPushButton:hover { "
                         "   background-color: %1; "
                         "   border: 2px solid %1; "
                         "   border-radius: %2px; "
                         "   font-size: %3px; "
                         "   padding: %4px; "
                         "   padding-left: %5px; "
                         "   padding-right: %5px; "
                         "} ";

    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (const auto &button, buttons) {
        button->setStyleSheet(styleSheet.arg(borderColour.name())
                                  .arg(borderRadius)
                                  .arg(bodyFontSize)
                                  .arg(padding)
                                  .arg(paddingHorizontal));
        button->setMinimumHeight(
            std::max(borderRadius * 2,
                     button->fontMetrics().boundingRect(button->text()).height()
                         + 2 * padding + 2 * 2));

#ifdef Q_OS_MAC
        // Hack to get around weird button sizing issues when switching styles
        button->setVisible(false);
        button->setVisible(true);
#endif
    }

    QString textStyleSheet = "QLabel { "
                             "   color: palette(text); "
                             "   font-size: %1px; "
                             "   padding-top: %2px; "
                             "   padding-bottom: %2px; "
                             "}";
    _currentChoiceLabel->setStyleSheet(
        textStyleSheet.arg(bodyFontSize).arg(padding));
    _currentChoiceLabel->setMinimumHeight(
        std::max(borderRadius * 2,
                 _currentChoiceLabel->fontMetrics()
                         .boundingRect(_currentChoiceLabel->text())
                         .height()
                     + 2 * padding + 2 * 2));

    setMinimumHeight(std::max(_currentChoiceLabel->minimumHeight(),
                              _simplifiedButton->minimumHeight()));
}

void SearchOptionsRadioGroupBox::notifyMediator() const
{
    QPushButton *sender = static_cast<QPushButton *>(QObject::sender());
    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (const auto &button, buttons) {
        button->setChecked(button == sender);
    }

    std::string language = sender->text().toStdString();

    Settings::getSettings()->setValue("SearchOptionsRadioGroupBox/lastSelected",
                                      sender->property("data"));

    _mediator->setParameters(sender->property("data").value<SearchParameters>());
}

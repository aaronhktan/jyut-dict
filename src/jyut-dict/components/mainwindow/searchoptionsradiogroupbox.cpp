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

#include <QLocale>
#include <QStyle>
#include <QTimer>

SearchOptionsRadioGroupBox::SearchOptionsRadioGroupBox(ISearchOptionsMediator *mediator,
                                                       QWidget *parent) :
    QGroupBox(parent)
{
    _mediator = mediator;
    _settings = Settings::getSettings(this);

    setupUI();
    translateUI();

    SearchParameters lastSelected
        = _settings
              ->value("SearchOptionsRadioGroupBox/lastSelected",
                      QVariant::fromValue(SearchParameters::ENGLISH))
              .value<SearchParameters>();
    QList<QRadioButton *> buttons = this->findChildren<QRadioButton *>();
    foreach (const auto & button, buttons) {
        if (button->property("data").value<SearchParameters>() == lastSelected) {
            button->click();
        #ifdef Q_OS_MAC
            // Makes the button selection show up correctly on macOS
            button->setDown(true);
        #endif
            break;
        }
    }
}

void SearchOptionsRadioGroupBox::changeEvent(QEvent *event)
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
    QGroupBox::changeEvent(event);
}

void SearchOptionsRadioGroupBox::setOption(const Utils::ButtonOptionIndex index)
{
    if (index >= Utils::MAXIMUM_BUTTON_INDEX) {
        return;
    }

    QList<QRadioButton *> buttons = this->findChildren<QRadioButton *>();
    buttons.at(index)->click();
}

void SearchOptionsRadioGroupBox::setOption(const SearchParameters parameters)
{
    QList<QRadioButton *> buttons = this->findChildren<QRadioButton *>();
    foreach (const auto & button, buttons) {
        if (button->property("data") == QVariant::fromValue(parameters)) {
            button->click();
        }
    }
}

void SearchOptionsRadioGroupBox::setupUI()
{

    _layout = new QHBoxLayout{this};
    _layout->setContentsMargins(15, 0, 55, 0);
#ifdef Q_OS_WIN
    _layout->setSpacing(15);
#endif

    _simplifiedButton = new QRadioButton{};
    _traditionalButton = new QRadioButton{};
    _jyutpingButton = new QRadioButton{};
    _pinyinButton = new QRadioButton{};
    _englishButton = new QRadioButton{};

    _simplifiedButton->setProperty("data", QVariant::fromValue(SearchParameters::SIMPLIFIED));
    _traditionalButton->setProperty("data", QVariant::fromValue(SearchParameters::TRADITIONAL));
    _jyutpingButton->setProperty("data", QVariant::fromValue(SearchParameters::JYUTPING));
    _pinyinButton->setProperty("data", QVariant::fromValue(SearchParameters::PINYIN));
    _englishButton->setProperty("data", QVariant::fromValue(SearchParameters::ENGLISH));

    connect(_simplifiedButton, &QRadioButton::clicked, this, &SearchOptionsRadioGroupBox::notifyMediator);
    connect(_traditionalButton, &QRadioButton::clicked, this, &SearchOptionsRadioGroupBox::notifyMediator);
    connect(_jyutpingButton, &QRadioButton::clicked, this, &SearchOptionsRadioGroupBox::notifyMediator);
    connect(_pinyinButton, &QRadioButton::clicked, this, &SearchOptionsRadioGroupBox::notifyMediator);
    connect(_englishButton, &QRadioButton::clicked, this, &SearchOptionsRadioGroupBox::notifyMediator);

    _layout->addWidget(_simplifiedButton);
    _layout->addWidget(_traditionalButton);
    _layout->addWidget(_jyutpingButton);
    _layout->addWidget(_pinyinButton);
    _layout->addWidget(_englishButton);

    setLayout(_layout);
    setFlat(true);
}

void SearchOptionsRadioGroupBox::translateUI()
{
    // Set property so styling automatically changes
    setProperty("isHan", Settings::isCurrentLocaleHan());

    QList<QRadioButton *> buttons = this->findChildren<QRadioButton *>();
    foreach (const auto & button, buttons) {
        button->setProperty("isHan", Settings::isCurrentLocaleHan());
        button->style()->unpolish(button);
        button->style()->polish(button);
    }

    _simplifiedButton->setText(tr("SC"));
    _traditionalButton->setText(tr("TC"));
    _jyutpingButton->setText(tr("JP"));
    _pinyinButton->setText(tr("PY"));
    _englishButton->setText(tr("EN"));

    _simplifiedButton->setToolTip(tr("Search Simplified Chinese"));
    _traditionalButton->setToolTip(tr("Search Traditional Chinese"));
    _jyutpingButton->setToolTip(tr("Search Jyutping"));
    _pinyinButton->setToolTip(tr("Search Pinyin"));
    _englishButton->setToolTip(tr("Search English"));
}

void SearchOptionsRadioGroupBox::setStyle(bool use_dark)
{
    (void) (use_dark);

    int interfaceSize = static_cast<int>(
        _settings
            ->value("Interface/size",
                    QVariant::fromValue(Settings::InterfaceSize::NORMAL))
            .value<Settings::InterfaceSize>());

    int h6FontSizeHan = Settings::h6FontSizeHan.at(
        static_cast<unsigned long>(interfaceSize - 1));

#ifdef Q_OS_WIN
    int h6FontSize = Settings::h6FontSize.at(
        static_cast<unsigned long>(interfaceSize - 1));

    setStyleSheet(QString{"QRadioButton[isHan=\"true\"] { "
                          "   font-size: %1px; "
                          "} "
                          ""
                          "QRadioButton[isHan=\"false\"] { "
                          "   font-size: %2px; "
                          "} "
                          ""
                          "QGroupBox { "
                          "   border: none; "
                          "} "}
                      .arg(h6FontSizeHan)
                      .arg(bodyFontSize));
#else
    int h6FontSize = Settings::h6FontSize.at(
        static_cast<unsigned long>(interfaceSize - 1));

    setStyleSheet(QString{"QRadioButton[isHan=\"true\"] { "
                          "   font-size: %1px; "
                          "} "
                          ""
                          "QRadioButton[isHan=\"false\"] { "
                          "   font-size: %2px; "
                          "} "
                          ""
                          "QGroupBox { "
                          "   border: none; "
                          "} "}
                      .arg(h6FontSizeHan)
                      .arg(h6FontSize));
#endif
}

void SearchOptionsRadioGroupBox::notifyMediator() const
{
    QRadioButton *button = static_cast<QRadioButton *>(QObject::sender());

    std::string language = button->text().toStdString();

    Settings::getSettings()->setValue("SearchOptionsRadioGroupBox/lastSelected",
                                      button->property("data"));

    _mediator->setParameters(button->property("data").value<SearchParameters>());
}

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
#include "logic/search/sqlsearch.h"
#include "logic/utils/utils_qt.h"

#include <QEvent>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QLocale>
#include <QPushButton>
#include <QScrollArea>
#include <QStyle>
#include <QTimer>
#include <QWidget>

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
                      QVariant::fromValue(SearchParameters::AUTO_DETECT))
              .value<SearchParameters>();
    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (const auto & button, buttons) {
        if (button->property("data").isValid()
            && button->property("data").value<SearchParameters>()
                   == lastSelected) {
            button->click();
            _scrollArea->ensureWidgetVisible(button);
            break;
        }
    }

    connect(this,
            &SearchOptionsRadioGroupBox::detectedLanguageChanged,
            this,
            &SearchOptionsRadioGroupBox::changeAutoDetectText);
}

SearchOptionsRadioGroupBox::~SearchOptionsRadioGroupBox()
{
    _search->deregisterObserver(this);
}

void SearchOptionsRadioGroupBox::detectedLanguage(SearchParameters params)
{
    emit detectedLanguageChanged(params);
}

void SearchOptionsRadioGroupBox::changeEvent(QEvent *event)
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
    }
    QGroupBox::changeEvent(event);
}

void SearchOptionsRadioGroupBox::setOption(const SearchParameters parameters)
{
    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (const auto & button, buttons) {
        if (button->property("data").isValid()
            && button->property("data") == QVariant::fromValue(parameters)) {
            button->click();
            _scrollArea->ensureWidgetVisible(button);
        }
    }
    if (parameters != SearchParameters::AUTO_DETECT) {
        animateAutoDetectText(_autoDetectButton,
                              QString::fromStdString(
                                  Utils::getStringFromSearchParameter(
                                      SearchParameters::AUTO_DETECT)));
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

    _scrollArea = new QScrollArea{this};
    _scrollArea->setFrameShape(QFrame::NoFrame);
    _scrollArea->setWidgetResizable(true);
    _scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _innerWidget = new QWidget{this};
    _innerWidget->setAttribute(Qt::WA_TranslucentBackground, true);
    _innerWidgetLayout = new QHBoxLayout{_innerWidget};
    _innerWidgetLayout->setContentsMargins(0, 0, 0, 0);
#ifdef Q_OS_LINUX
    _innerWidgetLayout->setSpacing(5);
#else
    _innerWidgetLayout->setSpacing(10);
#endif

    _currentChoiceLabel = new QLabel{this};
    _autoDetectButton = new QPushButton{this};
    _traditionalButton = new QPushButton{this};
    _simplifiedButton = new QPushButton{this};
    _jyutpingButton = new QPushButton{this};
    _fuzzyJyutpingButton = new QPushButton{this};
    _pinyinButton = new QPushButton{this};
    _fuzzyPinyinButton = new QPushButton{this};
    _englishButton = new QPushButton{this};

    _currentChoiceLabel->setIndent(0);
    _autoDetectButton->setCheckable(true);
    _traditionalButton->setCheckable(true);
    _simplifiedButton->setCheckable(true);
    _jyutpingButton->setCheckable(true);
    _fuzzyJyutpingButton->setCheckable(true);
    _pinyinButton->setCheckable(true);
    _fuzzyPinyinButton->setCheckable(true);
    _englishButton->setCheckable(true);

    _autoDetectButton->setCheckable(true);
    _autoDetectButton->setProperty("data",
                                   QVariant::fromValue(
                                       SearchParameters::AUTO_DETECT));
    _traditionalButton->setProperty("data",
                                    QVariant::fromValue(
                                        SearchParameters::TRADITIONAL));
    _simplifiedButton->setProperty("data",
                                   QVariant::fromValue(
                                       SearchParameters::SIMPLIFIED));
    _jyutpingButton->setProperty("data",
                                 QVariant::fromValue(
                                     SearchParameters::JYUTPING));
    _fuzzyJyutpingButton->setProperty("data",
                                      QVariant::fromValue(
                                          SearchParameters::FUZZY_JYUTPING));
    _pinyinButton->setProperty("data",
                               QVariant::fromValue(SearchParameters::PINYIN));
    _fuzzyPinyinButton->setProperty("data",
                                    QVariant::fromValue(
                                        SearchParameters::FUZZY_PINYIN));
    _englishButton->setProperty("data",
                                QVariant::fromValue(SearchParameters::ENGLISH));

    connect(_autoDetectButton,
            &QPushButton::clicked,
            this,
            &SearchOptionsRadioGroupBox::notifyMediator);
    connect(_traditionalButton,
            &QPushButton::clicked,
            this,
            &SearchOptionsRadioGroupBox::notifyMediator);
    connect(_simplifiedButton,
            &QPushButton::clicked,
            this,
            &SearchOptionsRadioGroupBox::notifyMediator);
    connect(_jyutpingButton,
            &QPushButton::clicked,
            this,
            &SearchOptionsRadioGroupBox::notifyMediator);
    connect(_fuzzyJyutpingButton,
            &QPushButton::clicked,
            this,
            &SearchOptionsRadioGroupBox::notifyMediator);
    connect(_pinyinButton,
            &QPushButton::clicked,
            this,
            &SearchOptionsRadioGroupBox::notifyMediator);
    connect(_fuzzyPinyinButton,
            &QPushButton::clicked,
            this,
            &SearchOptionsRadioGroupBox::notifyMediator);
    connect(_englishButton,
            &QPushButton::clicked,
            this,
            &SearchOptionsRadioGroupBox::notifyMediator);

    _innerWidgetLayout->addWidget(_autoDetectButton);
    _innerWidgetLayout->addWidget(_traditionalButton);
    _innerWidgetLayout->addWidget(_simplifiedButton);
    _innerWidgetLayout->addWidget(_jyutpingButton);
    _innerWidgetLayout->addWidget(_fuzzyJyutpingButton);
    _innerWidgetLayout->addWidget(_pinyinButton);
    _innerWidgetLayout->addWidget(_fuzzyPinyinButton);
    _innerWidgetLayout->addWidget(_englishButton);
    _innerWidgetLayout->addStretch(1);
    _scrollArea->setWidget(_innerWidget);

#ifdef Q_OS_MAC
    _layout->addWidget(_currentChoiceLabel, 0, Qt::AlignLeft | Qt::AlignBottom);
#else
    _layout->addWidget(_currentChoiceLabel, 0);
#endif
    _layout->addWidget(_scrollArea);

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
        if (button->property("data").isValid()) {
            button->setText(
                QString::fromStdString(Utils::getStringFromSearchParameter(
                    button->property("data").value<SearchParameters>())));
        }
    }

    _currentChoiceLabel->setText(tr("Searching:"));

    _autoDetectButton->setToolTip(
        tr("Search and detect language automatically"));
    _traditionalButton->setToolTip(tr("Search Traditional Chinese"));
    _simplifiedButton->setToolTip(tr("Search Simplified Chinese"));
    _jyutpingButton->setToolTip(tr("Search Jyutping"));
    _fuzzyJyutpingButton->setToolTip(tr("Search Fuzzy Jyutping"));
    _pinyinButton->setToolTip(tr("Search Pinyin"));
    _fuzzyPinyinButton->setToolTip(tr("Search Fuzzy Pinyin"));
    _englishButton->setToolTip(tr("Search English"));
}

void SearchOptionsRadioGroupBox::setStyle(bool use_dark)
{
    QColor borderColour = use_dark
                              ? QColor{Utils::HEADER_BACKGROUND_COLOUR_DARK_R,
                                       Utils::HEADER_BACKGROUND_COLOUR_DARK_G,
                                       Utils::HEADER_BACKGROUND_COLOUR_DARK_B}
                              : QColor{Utils::CONTENT_BACKGROUND_COLOUR_LIGHT_R,
                                       Utils::CONTENT_BACKGROUND_COLOUR_LIGHT_G,
                                       Utils::CONTENT_BACKGROUND_COLOUR_LIGHT_B};
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
#ifdef Q_OS_MAC
                         "   border: 2px solid %1; "
#else
                         "   border: 1px solid %1; "
#endif
                         "   border-radius: %2px; "
                         "   font-size: %3px; "
                         "   padding: %4px; "
                         "   padding-left: %5px; "
                         "   padding-right: %5px; "
                         "} "
                         " "
                         "QPushButton:checked { "
                         "   background-color: %1; "
#ifdef Q_OS_MAC
                         "   border: 2px solid %1; "
#else
                         "   border: 1px solid %1; "
#endif
                         "   border-radius: %2px; "
                         "   font-size: %3px; "
                         "   padding: %4px; "
                         "   padding-left: %5px; "
                         "   padding-right: %5px; "
                         "} "
                         " "
                         "QPushButton:hover { "
                         "   background-color: %1; "
#ifdef Q_OS_MAC
                         "   border: 2px solid %1; "
#else
                         "   border: 1px solid %1; "
#endif
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
        button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

#ifdef Q_OS_MAC
        // Hack to get around weird button sizing issues when switching styles
        button->setVisible(false);
        button->setVisible(true);
#endif
    }

    _scrollArea->setFixedHeight(_innerWidget->sizeHint().height() + 2);

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

void SearchOptionsRadioGroupBox::animateAutoDetectText(QPushButton *button,
                                                       const QString &newText)
{
    if (newText == button->text() || newText == button->property("newText")) {
        return;
    }
    button->setProperty("newText", newText);

    const int oldWidth = button->width();
    const QString oldText = button->text();
    button->setText(newText);
    const int newWidth = button->sizeHint().width();
    button->setText(oldText);

    QGraphicsOpacityEffect *opacity = qobject_cast<QGraphicsOpacityEffect *>(
        button->graphicsEffect());
    if (!opacity) {
        opacity = new QGraphicsOpacityEffect(button);
        button->setGraphicsEffect(opacity);
    }

    auto animate = [](QObject *target,
                      const char *property,
                      int duration,
                      QVariant start,
                      QVariant end) {
        QPropertyAnimation *a = new QPropertyAnimation(target, property);
        a->setDuration(duration);
        a->setStartValue(start);
        a->setEndValue(end);
        return a;
    };

    QPropertyAnimation *fadeOut = animate(opacity, "opacity", 75, 1.0, 0.0);
    QPropertyAnimation *fadeIn = animate(opacity, "opacity", 75, 0.0, 1.0);

    QParallelAnimationGroup *width = new QParallelAnimationGroup;
    width->addAnimation(
        animate(button, "minimumWidth", 150, oldWidth, newWidth));
    width->addAnimation(
        animate(button, "maximumWidth", 150, oldWidth, newWidth));

    QSequentialAnimationGroup *fadeInOut = new QSequentialAnimationGroup;
    fadeInOut->addAnimation(fadeOut);
    fadeInOut->addAnimation(width);
    fadeInOut->addAnimation(fadeIn);

    connect(fadeOut, &QAbstractAnimation::finished, button, [button, newText] {
        button->setText(newText);
    });

    connect(fadeInOut, &QAbstractAnimation::finished, button, [button] {
        button->setFixedSize(button->sizeHint());
        button->updateGeometry();
        button->setGraphicsEffect(nullptr);
        button->update();
    });

    fadeInOut->start(QAbstractAnimation::DeleteWhenStopped);
}

void SearchOptionsRadioGroupBox::notifyMediator()
{
    QPushButton *sender = static_cast<QPushButton *>(QObject::sender());
    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (const auto &button, buttons) {
        button->setChecked(button == sender);
    }

    Settings::getSettings()->setValue("SearchOptionsRadioGroupBox/lastSelected",
                                      sender->property("data"));

    SearchParameters params = sender->property("data").value<SearchParameters>();
    _mediator->setParameters(params);
    emit optionSelected(params);
}

void SearchOptionsRadioGroupBox::changeAutoDetectText(SearchParameters parameters)
{
    if (!_autoDetectButton->isChecked()) {
        return;
    }

    if (parameters == SearchParameters::NONE) {
        animateAutoDetectText(_autoDetectButton,
                              QString::fromStdString(
                                  Utils::getStringFromSearchParameter(
                                      SearchParameters::AUTO_DETECT)));
    } else {
        animateAutoDetectText(_autoDetectButton,
                              tr("Detected language: ")
                                  + QString::fromStdString(
                                      Utils::getStringFromSearchParameter(
                                          parameters)));
    }
}

#include "searchtab.h"

#include "logic/settings/settingsutils.h"
#include "logic/strings/strings.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QApplication>
#include <QDesktopServices>
#include <QFrame>
#include <QGridLayout>
#include <QPushButton>
#include <QStyle>
#include <QTimer>
#include <QUrl>
#include <QVariant>

SearchTab::SearchTab(QWidget *parent)
    : QWidget{parent}
{
    setObjectName("SearchTab");

    _settings = Settings::getSettings(this);
    setupUI();
    translateUI();
}

void SearchTab::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, this, [=, this]() {
            _paletteRecentlyChanged = false;
        });
        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QWidget::changeEvent(event);
}

void SearchTab::setupUI()
{
    _tabLayout = new QFormLayout{this};
#ifdef Q_OS_MAC
    _tabLayout->setContentsMargins(20, 20, 20, 20);
#elif defined(Q_OS_WIN)
    _tabLayout->setVerticalSpacing(15);
    _tabLayout->setContentsMargins(20, 20, 20, 20);
#elif defined(Q_OS_LINUX)
    _tabLayout->setVerticalSpacing(15);
    _tabLayout->setContentsMargins(20, 20, 20, 20);
    _tabLayout->setLabelAlignment(Qt::AlignRight);
#endif

    _searchAutoDetectCheckbox = new QCheckBox{this};
    _searchAutoDetectCheckbox->setTristate(false);
    initializeSearchAutoDetectCheckbox(*_searchAutoDetectCheckbox);

    QFrame *_divider = new QFrame{this};
    _divider->setObjectName("divider");
    _divider->setFrameShape(QFrame::HLine);
    _divider->setFrameShadow(QFrame::Raised);
    _divider->setFixedHeight(1);

    _jyutpingTitleLabel = new QLabel{this};

    _fuzzyJyutping = new QWidget{this};
    _fuzzyJyutpingLayout = new QGridLayout{_fuzzyJyutping};
    _fuzzyJyutpingLayout->setContentsMargins(0, 11, 0, 0);
    _fuzzyJyutpingCheckbox = new QCheckBox{this};
    _fuzzyJyutpingCheckbox->setTristate(false);
    _fuzzyJyutpingDescription = new QLabel{this};

    _dangerousFuzzyJyutping = new QWidget{this};
    _dangerousFuzzyJyutpingLayout = new QGridLayout{_dangerousFuzzyJyutping};
    _dangerousFuzzyJyutpingLayout->setContentsMargins(0, 11, 0, 0);
    _dangerousFuzzyJyutpingCheckbox = new QCheckBox{this};
    _dangerousFuzzyJyutpingCheckbox->setTristate(false);
    _dangerousFuzzyJyutpingDescription = new QLabel{this};

    _tabLayout->addRow(" ", _searchAutoDetectCheckbox);

    _tabLayout->addRow(_divider);

    _tabLayout->addRow(_jyutpingTitleLabel);
    _tabLayout->addRow(" ", _fuzzyJyutping);
    _tabLayout->addRow(" ", _dangerousFuzzyJyutping);

    initializeFuzzyJyutping(*_fuzzyJyutping);
    initializeDangerousFuzzyJyutping(*_dangerousFuzzyJyutping);

    _dangerousFuzzyJyutpingCheckbox->setEnabled(
        _fuzzyJyutpingCheckbox->checkState());

    // Set the style to match whether the user started dark mode
    setStyle(Utils::isDarkMode());
}

void SearchTab::translateUI()
{
    // Set property so styling automatically changes
    setProperty("isHan", Settings::isCurrentLocaleHan());

    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (const auto & button, buttons) {
        button->setProperty("isHan", Settings::isCurrentLocaleHan());
        button->style()->unpolish(button);
        button->style()->polish(button);
    }

    static_cast<QLabel *>(_tabLayout->labelForField(_searchAutoDetectCheckbox))
        ->setText(tr("Auto-detect search language:"));

    _jyutpingTitleLabel->setText("<b>" + tr("Jyutping:") + "</b>");
    static_cast<QLabel *>(_tabLayout->labelForField(_fuzzyJyutping))
        ->setText(tr("Fuzzy Jyutping matching:"));
    static_cast<QLabel *>(_tabLayout->labelForField(_dangerousFuzzyJyutping))
#ifdef Q_OS_WIN
        ->setText(tr("Risky fuzzy Jyutping matches:"));
#else
        ->setText(tr("\tRisky fuzzy Jyutping matches:"));
#endif

    QColor backgroundColour = Utils::isDarkMode()
                                  ? QColor{LABEL_TEXT_COLOUR_DARK_R,
                                           LABEL_TEXT_COLOUR_DARK_G,
                                           LABEL_TEXT_COLOUR_DARK_B}
                                  : QColor{LABEL_TEXT_COLOUR_LIGHT_R,
                                           LABEL_TEXT_COLOUR_LIGHT_R,
                                           LABEL_TEXT_COLOUR_LIGHT_R};

    _fuzzyJyutpingDescription->setText(
        QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                    Strings::FUZZY_JYUTPING_EXPLAINER)
            .arg(backgroundColour.name()));
    _fuzzyJyutpingDescription->setWordWrap(true);
    _dangerousFuzzyJyutpingDescription->setText(
        QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                    Strings::DANGEROUS_FUZZY_JYUTPING_EXPLAINER)
            .arg(backgroundColour.name()));
    _dangerousFuzzyJyutpingDescription->setWordWrap(true);
}

void SearchTab::setStyle(bool use_dark)
{
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet("QWidget#SearchTab { "
                  "   background-color: palette(base); "
                  "} ");

    QString colour = use_dark ? "#424242" : "#d5d5d5";
    QString style = "QFrame { border: 1px solid %1; }";
    QList<QFrame *> frames = this->findChildren<QFrame *>("divider");
    foreach (const auto & frame, frames) {
        frame->setStyleSheet(style.arg(colour));
    }

    QColor backgroundColour = use_dark ? QColor{LABEL_TEXT_COLOUR_DARK_R,
                                                LABEL_TEXT_COLOUR_DARK_G,
                                                LABEL_TEXT_COLOUR_DARK_B}
                                       : QColor{LABEL_TEXT_COLOUR_LIGHT_R,
                                                LABEL_TEXT_COLOUR_LIGHT_R,
                                                LABEL_TEXT_COLOUR_LIGHT_R};

    _fuzzyJyutpingDescription->setText(
        QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                    Strings::FUZZY_JYUTPING_EXPLAINER)
            .arg(backgroundColour.name()));
    _fuzzyJyutpingDescription->setWordWrap(true);
    _fuzzyJyutpingDescription->setMinimumWidth(300);
    _dangerousFuzzyJyutpingDescription->setText(
        QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                    Strings::DANGEROUS_FUZZY_JYUTPING_EXPLAINER)
            .arg(backgroundColour.name()));
    _dangerousFuzzyJyutpingDescription->setWordWrap(true);
    _dangerousFuzzyJyutpingDescription->setMinimumWidth(300);
}

void SearchTab::initializeSearchAutoDetectCheckbox(QCheckBox &checkbox)
{
    connect(&checkbox, &QCheckBox::checkStateChanged, this, [&]() {
        _settings->setValue("Search/autoDetectLanguage", checkbox.checkState());
        _settings->sync();
        emit triggerSearch();
    });

    setSearchAutoDetectCheckboxDefault(checkbox);
}

void SearchTab::initializeFuzzyJyutping(QWidget &widget)
{
    static_cast<QGridLayout *>(widget.layout())
        ->addWidget(_fuzzyJyutpingCheckbox, 0, 0, 1, -1);
    static_cast<QGridLayout *>(widget.layout())
        ->addWidget(_fuzzyJyutpingDescription, 1, 0, 1, -1);

    connect(_fuzzyJyutpingCheckbox, &QCheckBox::checkStateChanged, this, [&]() {
        _settings->setValue("Search/fuzzyJyutping",
                            _fuzzyJyutpingCheckbox->checkState());
        _settings->sync();
        _dangerousFuzzyJyutpingCheckbox->setEnabled(
            _fuzzyJyutpingCheckbox->checkState());
        emit triggerSearch();
    });
    setFuzzyJyutpingCheckboxDefault(*_fuzzyJyutpingCheckbox);
}

void SearchTab::initializeDangerousFuzzyJyutping(QWidget &widget)
{
    static_cast<QGridLayout *>(widget.layout())
        ->addWidget(_dangerousFuzzyJyutpingCheckbox, 0, 0, 1, -1);
    static_cast<QGridLayout *>(widget.layout())
        ->addWidget(_dangerousFuzzyJyutpingDescription, 1, 0, 1, -1);

    connect(_dangerousFuzzyJyutpingCheckbox,
            &QCheckBox::checkStateChanged,
            this,
            [&]() {
                _settings
                    ->setValue("Search/dangerousFuzzyJyutping",
                               _dangerousFuzzyJyutpingCheckbox->checkState());
                _settings->sync();
                emit triggerSearch();
            });
    setDangerousFuzzyJyutpingCheckboxDefault(*_dangerousFuzzyJyutpingCheckbox);
}

void SearchTab::setSearchAutoDetectCheckboxDefault(QCheckBox &checkbox)
{
    checkbox.setChecked(
        _settings->value("Search/autoDetectLanguage", QVariant{true}).toBool());
}

void SearchTab::setFuzzyJyutpingCheckboxDefault(QCheckBox &checkbox)
{
    checkbox.setChecked(
        _settings->value("Search/fuzzyJyutping", QVariant{true}).toBool());
}

void SearchTab::setDangerousFuzzyJyutpingCheckboxDefault(QCheckBox &checkbox)
{
    checkbox.setChecked(
        _settings->value("Search/dangerousFuzzyJyutping", QVariant{false})
            .toBool());
}

void SearchTab::resetSettings(void)
{
    setSearchAutoDetectCheckboxDefault(*_searchAutoDetectCheckbox);
    setFuzzyJyutpingCheckboxDefault(*_fuzzyJyutpingCheckbox);
    setFuzzyJyutpingCheckboxDefault(*_dangerousFuzzyJyutpingCheckbox);
}

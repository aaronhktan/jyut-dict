#include "advancedtab.h"

#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#endif

#include <QApplication>
#include <QFrame>
#include <QLibraryInfo>
#include <QTimer>

AdvancedTab::AdvancedTab(QWidget *parent)
    : QWidget(parent)
{
    _settings = Settings::getSettings();
    _analytics = new Analytics{this};

    setupUI();
    translateUI();
}

void AdvancedTab::changeEvent(QEvent *event)
{
#if defined(Q_OS_DARWIN)
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(100, [=]() { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
#endif
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QWidget::changeEvent(event);
}

void AdvancedTab::setupUI()
{
    _tabLayout = new QFormLayout{this};
#ifdef Q_OS_MAC
    _tabLayout->setFormAlignment(Qt::AlignHCenter | Qt::AlignCenter);
    _tabLayout->setContentsMargins(20, 20, 20, 20);
#elif defined(Q_OS_WIN)
    _tabLayout->setVerticalSpacing(15);
    _tabLayout->setContentsMargins(20, 20, 20, 20);
#elif defined(Q_OS_LINUX)
    _tabLayout->setVerticalSpacing(15);
    _tabLayout->setContentsMargins(20, 20, 20, 20);
    _tabLayout->setLabelAlignment(Qt::AlignRight);
#endif

    _updateCheckbox = new QCheckBox{this};
    _updateCheckbox->setTristate(false);
    initializeUpdateCheckbox(*_updateCheckbox);

    _analyticsCheckbox = new QCheckBox{this};
    _analyticsCheckbox->setTristate(false);
    initializeAnalyticsCheckbox(*_analyticsCheckbox);

    QFrame *_divider = new QFrame{this};
    _divider->setObjectName("divider");
    _divider->setFrameShape(QFrame::HLine);
    _divider->setFrameShadow(QFrame::Raised);
    _divider->setFixedHeight(1);

    _languageCombobox = new QComboBox{this};
    _languageCombobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    initializeLanguageCombobox(*_languageCombobox);

    _tabLayout->addRow(" ", _updateCheckbox);
    _tabLayout->addRow(" ", _analyticsCheckbox);
    _tabLayout->addRow(_divider);
    _tabLayout->addRow(" ", _languageCombobox);

#ifdef Q_OS_MAC
    // Set the style to match whether the user started dark mode
    setStyle(Utils::isDarkMode());
#endif
}

void AdvancedTab::translateUI()
{
    static_cast<QLabel *>(_tabLayout->labelForField(_updateCheckbox))
        ->setText(tr("Automatically check for updates on startup:"));
    static_cast<QLabel *>(_tabLayout->labelForField(_analyticsCheckbox))
        ->setText(tr("Enable analytics:"));
    static_cast<QLabel *>(_tabLayout->labelForField(_languageCombobox))
        ->setText(tr("Application language:"));

    _languageCombobox->setItemText(0, tr("Use system language"));
    _languageCombobox->setItemText(1, tr("English"));
    _languageCombobox->setItemText(2, tr("French (CA)"));
    _languageCombobox->setItemText(3, tr("French"));
    _languageCombobox->setItemText(4, tr("Cantonese (Simplified)"));
    _languageCombobox->setItemText(5, tr("Cantonese (Traditional)"));
    _languageCombobox->setItemText(6, tr("Simplified Chinese"));
    _languageCombobox->setItemText(7, tr("Traditional Chinese"));
}

void AdvancedTab::setStyle(bool use_dark)
{
    QString colour = use_dark ? "#424242" : "#d5d5d5";
    QString style = "QFrame { border: 1px solid %1; }";
    QList<QFrame *> frames
        = this->findChildren<QFrame *>("divider");
    for (auto frame : frames) {
        frame->setStyleSheet(style.arg(colour));
    }
}

void AdvancedTab::initializeUpdateCheckbox(QCheckBox &checkbox)
{
    checkbox.setChecked(
        _settings->value("Advanced/updateNotificationsEnabled", QVariant{true}).toBool());

    connect(&checkbox, &QCheckBox::stateChanged, this, [&]() {
        _analytics->sendEvent("settings", "updateNotificationsEnabled",
                              checkbox.checkState() ? "true": "false");
        _settings->setValue("Advanced/updateNotificationsEnabled",
                            checkbox.checkState());
        _settings->sync();
    });
}

void AdvancedTab::initializeAnalyticsCheckbox(QCheckBox &checkbox)
{
    checkbox.setChecked(
        _settings->value("Advanced/analyticsEnabled", QVariant{true}).toBool());

    connect(&checkbox, &QCheckBox::stateChanged, this, [&]() {
        _analytics->sendEvent("settings", "analyticsEnabled",
                              checkbox.checkState() ? "true": "false");
        _settings->setValue("Advanced/analyticsEnabled",
                            checkbox.checkState());
        _settings->sync();
    });
}

void AdvancedTab::initializeLanguageCombobox(QComboBox &combobox)
{
    combobox.addItem("0", "system");
    combobox.addItem("1", "en");
    combobox.addItem("2", "fr_CA");
    combobox.addItem("3", "fr");
    combobox.addItem("4", "yue_Hans");
    combobox.addItem("5", "yue_Hant");
    combobox.addItem("6", "zh_Hans");
    combobox.addItem("7", "zh_Hant");

    combobox.setCurrentIndex(combobox.findData(
        _settings->value("Advanced/locale", QVariant{"system"}).toString()));

    connect(&combobox,
            QOverload<int>::of(&QComboBox::activated),
            this,
            [&](int index) {
                QString localeName = combobox.itemData(index).toString();
                QLocale newLocale = Settings::getCurrentLocale();
                if (localeName == "system") {
                    newLocale = QLocale{};
                    _settings->remove("Advanced/locale");
                } else {
                    newLocale = QLocale{combobox.itemData(index).toString()};
                    _settings->setValue("Advanced/locale",
                                        combobox.itemData(index));
                }

                _settings->sync();
                Settings::setCurrentLocale(newLocale);
                _analytics->sendEvent("settings", "locale",
                                      newLocale.name().toStdString());

                qApp->removeTranslator(&Settings::systemTranslator);
                Settings::systemTranslator
                    .load("qt_" + newLocale.name(),
                          QLibraryInfo::location(
                              QLibraryInfo::TranslationsPath));
                qApp->installTranslator(&Settings::systemTranslator);

                qApp->removeTranslator(&Settings::applicationTranslator);
                Settings::applicationTranslator
                    .load(/* QLocale */ newLocale,
                          /* filename */ "jyutdictionary",
                          /* prefix */ "-",
                          /* directory */ ":/translations");
                qApp->installTranslator(&Settings::applicationTranslator);
            });
}
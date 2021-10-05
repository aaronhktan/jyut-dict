#include "advancedtab.h"

#include "logic/database/sqldatabasemanager.h"
#include "logic/settings/settingsutils.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif

#include <QApplication>
#include <QFileDialog>
#include <QFrame>
#include <QLibraryInfo>
#include <QTimer>
#include <QtConcurrent/QtConcurrent>
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
#include <QWindow>
#endif

AdvancedTab::AdvancedTab(QWidget *parent)
    : QWidget{parent}
{
    _settings = Settings::getSettings();

    setupUI();
    translateUI();
}

void AdvancedTab::changeEvent(QEvent *event)
{

    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(100, this, [=]() { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }

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

#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    _forceDarkModeCheckbox = new QCheckBox{this};
    _forceDarkModeCheckbox->setTristate(false);
    initializeForceDarkModeCheckbox(*_forceDarkModeCheckbox);
#endif

    QFrame *_exportDivider = new QFrame{this};
    _exportDivider->setObjectName("divider");
    _exportDivider->setFrameShape(QFrame::HLine);
    _exportDivider->setFrameShadow(QFrame::Raised);
    _exportDivider->setFixedHeight(1);

    _exportUserDatabaseButton = new QPushButton{this};
    connect(_exportUserDatabaseButton,
            &QPushButton::clicked,
            this,
            &AdvancedTab::exportUserDatabase);

    QFrame *_divider = new QFrame{this};
    _divider->setObjectName("divider");
    _divider->setFrameShape(QFrame::HLine);
    _divider->setFrameShadow(QFrame::Raised);
    _divider->setFixedHeight(1);

    _languageCombobox = new QComboBox{this};
    _languageCombobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    initializeLanguageCombobox(*_languageCombobox);

    _tabLayout->addRow(" ", _updateCheckbox);
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    _tabLayout->addRow(" ", _forceDarkModeCheckbox);
#endif
    _tabLayout->addRow(_exportDivider);
    _tabLayout->addRow(" ", _exportUserDatabaseButton);
    _tabLayout->addRow(_divider);
    _tabLayout->addRow(" ", _languageCombobox);

    // Set the style to match whether the user started dark mode
    setStyle(Utils::isDarkMode());
}

void AdvancedTab::translateUI()
{
    setProperty("isHan", Settings::isCurrentLocaleHan());

    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    for (auto button : buttons) {
        button->setProperty("isHan", Settings::isCurrentLocaleHan());
        button->style()->unpolish(button);
        button->style()->polish(button);
    }

    static_cast<QLabel *>(_tabLayout->labelForField(_updateCheckbox))
        ->setText(tr("Automatically check for updates on startup:"));
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    static_cast<QLabel *>(_tabLayout->labelForField(_forceDarkModeCheckbox))
        ->setText(tr("Enable dark mode:"));
#endif
    static_cast<QLabel *>(_tabLayout->labelForField(_exportUserDatabaseButton))
        ->setText(tr("Back up saved words and history:"));
    static_cast<QLabel *>(_tabLayout->labelForField(_languageCombobox))
        ->setText(tr("Application language:"));

    _exportUserDatabaseButton->setText(tr("Back up"));

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
#ifdef Q_OS_MAC
    setStyleSheet("QPushButton[isHan=\"true\"] { font-size: "
                  "13px; height: 16px; }");
#elif defined(Q_OS_WIN)
    setAttribute(Qt::WA_StyledBackground);
    setObjectName("AdvancedTab");
    setStyleSheet("QPushButton[isHan=\"true\"] { "
                  "   font-size: 12px; height: 20px; "
                  "} "
                  ""
                  "QWidget#AdvancedTab { "
                  "   background-color: palette(base);"
                  "} ");
#endif
}

void AdvancedTab::initializeUpdateCheckbox(QCheckBox &checkbox)
{
    checkbox.setChecked(
        _settings->value("Advanced/updateNotificationsEnabled", QVariant{true}).toBool());

    connect(&checkbox, &QCheckBox::stateChanged, this, [&]() {
        _settings->setValue("Advanced/updateNotificationsEnabled",
                            checkbox.checkState());
        _settings->sync();
    });
}

#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
void AdvancedTab::initializeForceDarkModeCheckbox(QCheckBox &checkbox)
{
    checkbox.setChecked(
        _settings->value("Advanced/forceDarkMode", QVariant{false}).toBool());

    connect(&checkbox, &QCheckBox::stateChanged, this, [&]() {
        _settings->setValue("Advanced/forceDarkMode",
                            checkbox.checkState());
        _settings->sync();

        QEvent event{QEvent::PaletteChange};
        foreach (auto window, qApp->topLevelWindows()) {
            QCoreApplication::sendEvent(window, &event);
        }
    });
}
#endif

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

void AdvancedTab::exportUserDatabase(void)
{
    QFileDialog *_fileDialog = new QFileDialog{this};
    _fileDialog->setAcceptMode(QFileDialog::AcceptSave);

    QString destinationFileName
        = _fileDialog->getSaveFileName(this,
                                       tr("Path to save exported database"),
                                       QDir::homePath());
    if (!destinationFileName.toStdString().empty()) {
        if (!destinationFileName.endsWith(".db")) {
            destinationFileName = destinationFileName + ".db";
        }
        if (QFile::exists(destinationFileName)) {
            QFile::remove(destinationFileName);
        }
        SQLDatabaseManager manager;
        QtConcurrent::run(QFile::copy,
                          manager.getUserDatabasePath(),
                          destinationFileName);
    }
}

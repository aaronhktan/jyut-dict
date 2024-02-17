#include "advancedtab.h"

#include "dialogs/resetsettingsdialog.h"
#include "logic/database/sqldatabasemanager.h"
#include "logic/entry/entryspeaker.h"
#include "logic/settings/settingsutils.h"
#include "logic/strings/strings.h"
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
        QTimer::singleShot(10, this, [=]() { _paletteRecentlyChanged = false; });

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

    QFrame *_ttsDivider = new QFrame{this};
    _ttsDivider->setObjectName("divider");
    _ttsDivider->setFrameShape(QFrame::HLine);
    _ttsDivider->setFrameShadow(QFrame::Raised);
    _ttsDivider->setFixedHeight(1);

    _cantoneseTTSWidget = new QWidget{this};
    _cantoneseTTSLayout = new QHBoxLayout{_cantoneseTTSWidget};
    _cantoneseTTSLayout->setContentsMargins(0, 0, 0, 0);
    _useCantoneseQtTTSBackend = new QRadioButton{this};
    _useCantoneseQtTTSBackend->setProperty("data",
                                           QVariant::fromValue(
                                               SpeakerBackend::QT_TTS));
    _useCantoneseGoogleOfflineSyllableTTSBackend = new QRadioButton{this};
    _useCantoneseGoogleOfflineSyllableTTSBackend
        ->setProperty("data",
                      QVariant::fromValue(
                          SpeakerBackend::GOOGLE_OFFLINE_SYLLABLE_TTS));
    _useYue1Voice = new QRadioButton{this};
    _useYue1Voice->setProperty("data", QVariant::fromValue(Voice::YUE_1));
    _useYue2Voice = new QRadioButton{this};
    _useYue2Voice->setProperty("data", QVariant::fromValue(Voice::YUE_2));
    initializeCantoneseTTSWidget(*_cantoneseTTSWidget);

    _mandarinTTSWidget = new QWidget{this};
    _mandarinTTSLayout = new QHBoxLayout{_mandarinTTSWidget};
    _mandarinTTSLayout->setContentsMargins(0, 0, 0, 0);
    _useMandarinQtTTSBackend = new QRadioButton{this};
    _useMandarinQtTTSBackend->setProperty("data",
                                          QVariant::fromValue(
                                              SpeakerBackend::QT_TTS));
    _useMandarinGoogleOfflineSyllableTTSBackend = new QRadioButton{this};
    _useMandarinGoogleOfflineSyllableTTSBackend
        ->setProperty("data",
                      QVariant::fromValue(
                          SpeakerBackend::GOOGLE_OFFLINE_SYLLABLE_TTS));
    _useCmn1Voice = new QRadioButton{this};
    _useCmn1Voice->setProperty("data", QVariant::fromValue(Voice::CMN_1));
    _useCmn2Voice = new QRadioButton{this};
    _useCmn2Voice->setProperty("data", QVariant::fromValue(Voice::CMN_2));

    QFrame *_exportDivider = new QFrame{this};
    _exportDivider->setObjectName("divider");
    _exportDivider->setFrameShape(QFrame::HLine);
    _exportDivider->setFrameShadow(QFrame::Raised);
    _exportDivider->setFixedHeight(1);

    _exportDictionaryDatabaseButton = new QPushButton{this};
    connect(_exportDictionaryDatabaseButton,
            &QPushButton::clicked,
            this,
            &AdvancedTab::exportDictionaryDatabase);
    _exportUserDatabaseButton = new QPushButton{this};
    connect(_exportUserDatabaseButton,
            &QPushButton::clicked,
            this,
            &AdvancedTab::exportUserDatabase);

#ifndef Q_OS_WIN
    QFrame *_restoreDivider = new QFrame{this};
    _restoreDivider->setObjectName("divider");
    _restoreDivider->setFrameShape(QFrame::HLine);
    _restoreDivider->setFrameShadow(QFrame::Raised);
    _restoreDivider->setFixedHeight(1);

    _restoreBackedUpDictionaryDatabaseButton = new QPushButton{this};
    connect(_restoreBackedUpDictionaryDatabaseButton,
            &QPushButton::clicked,
            this,
            &AdvancedTab::restoreBackedUpDictionaryDatabase);
    _restoreExportedDictionaryDatabaseButton = new QPushButton{this};
    connect(_restoreExportedDictionaryDatabaseButton,
            &QPushButton::clicked,
            this,
            &AdvancedTab::restoreExportedDictionaryDatabase);
    _restoreExportedUserDatabaseButton = new QPushButton{this};
    connect(_restoreExportedUserDatabaseButton,
            &QPushButton::clicked,
            this,
            &AdvancedTab::restoreExportedUserDatabase);
#endif

    QFrame *_languageDivider = new QFrame{this};
    _languageDivider->setObjectName("divider");
    _languageDivider->setFrameShape(QFrame::HLine);
    _languageDivider->setFrameShadow(QFrame::Raised);
    _languageDivider->setFixedHeight(1);

    _languageCombobox = new QComboBox{this};
    _languageCombobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    initializeLanguageCombobox(*_languageCombobox);

    QFrame *_resetDivider = new QFrame{this};
    _resetDivider->setObjectName("divider");
    _resetDivider->setFrameShape(QFrame::HLine);
    _resetDivider->setFrameShadow(QFrame::Raised);
    _resetDivider->setFixedHeight(1);

    _resetButton = new QPushButton{this};
    _resetButton->setObjectName("reset button");
    initializeResetButton(*_resetButton);

    _tabLayout->addRow(" ", _updateCheckbox);
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    _tabLayout->addRow(" ", _forceDarkModeCheckbox);
#endif
    _tabLayout->addRow(_ttsDivider);
    _tabLayout->addRow(" ", _cantoneseTTSWidget);
    _tabLayout->addRow(" ", _mandarinTTSWidget);
    _tabLayout->addRow(_exportDivider);
    _tabLayout->addRow(" ", _exportDictionaryDatabaseButton);
    _tabLayout->addRow(" ", _exportUserDatabaseButton);
#ifndef Q_OS_WIN
    _tabLayout->addRow(_restoreDivider);
    _tabLayout->addRow(" ", _restoreBackedUpDictionaryDatabaseButton);
    _tabLayout->addRow(" ", _restoreExportedDictionaryDatabaseButton);
    _tabLayout->addRow(" ", _restoreExportedUserDatabaseButton);
#endif
    _tabLayout->addRow(_languageDivider);
    _tabLayout->addRow(" ", _languageCombobox);
    _tabLayout->addRow(_resetDivider);
    _tabLayout->addRow(_resetButton);
    _tabLayout->setAlignment(_resetButton, Qt::AlignRight);

    // Set the style to match whether the user started dark mode
    setStyle(Utils::isDarkMode());
}

void AdvancedTab::translateUI()
{
    setProperty("isHan", Settings::isCurrentLocaleHan());

    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (const auto & button, buttons) {
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
    static_cast<QLabel *>(_tabLayout->labelForField(_cantoneseTTSWidget))
        ->setText(tr("Cantonese text-to-speech:"));
    _useCantoneseQtTTSBackend->setText(tr("Qt"));
    _useCantoneseGoogleOfflineSyllableTTSBackend->setText(tr("Google"));
    static_cast<QLabel *>(_tabLayout->labelForField(_mandarinTTSWidget))
        ->setText(tr("Mandarin text-to-speech:"));
    _useMandarinQtTTSBackend->setText(tr("Qt"));
    _useMandarinGoogleOfflineSyllableTTSBackend->setText(tr("Google"));
    static_cast<QLabel *>(
        _tabLayout->labelForField(_exportDictionaryDatabaseButton))
        ->setText(tr("Export dictionaries file:"));
    static_cast<QLabel *>(_tabLayout->labelForField(_exportUserDatabaseButton))
        ->setText(tr("Export saved words and history:"));
    _exportDictionaryDatabaseButton->setText(tr("Export"));
    _exportUserDatabaseButton->setText(tr("Export"));

#ifndef Q_OS_WIN
    static_cast<QLabel *>(_tabLayout->labelForField(_restoreBackedUpDictionaryDatabaseButton))
        ->setText(tr("Restore dictionaries file to last backed up version:"));
    static_cast<QLabel *>(_tabLayout->labelForField(_restoreExportedDictionaryDatabaseButton))
        ->setText(tr("Restore dictionaries file using exported file:"));
    static_cast<QLabel *>(_tabLayout->labelForField(_restoreExportedUserDatabaseButton))
        ->setText(tr("Restore saved words and history file using exported file:"));
    _restoreBackedUpDictionaryDatabaseButton->setText(tr("Restore"));
    _restoreExportedDictionaryDatabaseButton->setText(tr("Restore"));
    _restoreExportedUserDatabaseButton->setText(tr("Restore"));
#endif

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

    _resetButton->setText(tr("Reset all settings"));
}

void AdvancedTab::setStyle(bool use_dark)
{
    QString colour = use_dark ? "#424242" : "#d5d5d5";
    QString style = "QFrame { border: 1px solid %1; }";
    QList<QFrame *> frames
        = this->findChildren<QFrame *>("divider");
    foreach (const auto & frame, frames) {
        frame->setStyleSheet(style.arg(colour));
    }
#ifdef Q_OS_MAC
    setStyleSheet("QPushButton[isHan=\"true\"] { font-size: "
                  "13px; height: 16px; }");
#elif defined(Q_OS_LINUX) || defined(Q_OS_WIN)
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
    connect(&checkbox, &QCheckBox::stateChanged, this, [&]() {
        _settings->setValue("Advanced/updateNotificationsEnabled",
                            checkbox.checkState());
        _settings->sync();
    });

    setUpdateCheckboxDefault(checkbox);
}

#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
void AdvancedTab::initializeForceDarkModeCheckbox(QCheckBox &checkbox)
{
    setForceDarkModeCheckboxDefault(checkbox);

    connect(&checkbox, &QCheckBox::stateChanged, this, [&]() {
        _settings->setValue("Advanced/forceDarkMode",
                            checkbox.checkState());
        _settings->sync();

        QEvent event{QEvent::PaletteChange};
        foreach (const auto & window, qApp->topLevelWindows()) {
            QCoreApplication::sendEvent(window, &event);
        }
    });
}
#endif

void AdvancedTab::initializeCantoneseTTSWidget(QWidget &widget)
{
    widget.layout()->addWidget(_useCantoneseQtTTSBackend);
    widget.layout()->addWidget(_useCantoneseGoogleOfflineSyllableTTSBackend);

    connect(_useCantoneseQtTTSBackend, &QRadioButton::clicked, this, [&]() {
        _settings->setValue("Advanced/CantoneseSpeakerBackend",
                            QVariant::fromValue<SpeakerBackend>(
                                SpeakerBackend::QT_TTS));
        _settings->setValue("Advanced/CantoneseSpeakerVoice",
                            QVariant::fromValue<Voice>(Voice::NONE));
        _settings->sync();
    });

    connect(_useCantoneseGoogleOfflineSyllableTTSBackend,
            &QRadioButton::clicked,
            this,
            [&]() {
                _settings
                    ->setValue("Advanced/CantoneseSpeakerBackend",
                               QVariant::fromValue<SpeakerBackend>(
                                   SpeakerBackend::GOOGLE_OFFLINE_SYLLABLE_TTS));
                _settings->setValue("Advanced/CantoneseSpeakerVoice",
                                    QVariant::fromValue<Voice>(Voice::YUE_1));
                _settings->sync();
            });

    setCantoneseTTSWidgetDefault(widget);
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

    setLanguageComboboxDefault(combobox);
}

void AdvancedTab::initializeResetButton(QPushButton &resetButton)
{
    connect(&resetButton, &QPushButton::clicked, this, [&]() {
        ResetSettingsDialog *_message = new ResetSettingsDialog{this};
        if (_message->exec() == QMessageBox::Yes) {
            resetSettings(*_settings);
        }
    });

    resetButton.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
}

void AdvancedTab::setUpdateCheckboxDefault(QCheckBox &checkbox)
{
    checkbox.setChecked(
        _settings->value("Advanced/updateNotificationsEnabled", QVariant{true})
            .toBool());
}

#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
void AdvancedTab::setForceDarkModeCheckboxDefault(QCheckBox &checkbox)
{
    checkbox.setChecked(
        _settings->value("Advanced/forceDarkMode", QVariant{false}).toBool());
}
#endif

void AdvancedTab::setCantoneseTTSWidgetDefault(QWidget &widget)
{
    SpeakerBackend backend = Settings::getSettings()
                                 ->value("Advanced/CantoneseSpeakerBackend",
                                         QVariant::fromValue(
                                             SpeakerBackend::QT_TTS))
                                 .value<SpeakerBackend>();

    QList<QRadioButton *> buttons = widget.findChildren<QRadioButton *>();
    foreach (const auto &button, buttons) {
        if (button->property("data").value<SpeakerBackend>() == backend) {
            button->click();
#ifdef Q_OS_MAC
            // Makes the button selection show up correctly on macOS
            button->setDown(true);
#endif
            break;
        }
    }
}

void AdvancedTab::setLanguageComboboxDefault(QComboBox &combobox)
{
    combobox.setCurrentIndex(combobox.findData(
        _settings->value("Advanced/locale", QVariant{"system"}).toString()));
}

void AdvancedTab::resetSettings(QSettings &settings)
{
    Settings::clearSettings(settings);

    setUpdateCheckboxDefault(*_updateCheckbox);
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    setForceDarkModeCheckboxDefault(*_forceDarkModeCheckbox);
#endif
    setLanguageComboboxDefault(*_languageCombobox);

    emit settingsReset();
}

void AdvancedTab::exportDictionaryDatabase(void)
{
    QFileDialog *_fileDialog = new QFileDialog{this};
    _fileDialog->setAcceptMode(QFileDialog::AcceptSave);

    QString destinationFileName
        = _fileDialog->getSaveFileName(this,
                                       tr("Path to save exported database"),
                                       QDir::homePath());
    if (destinationFileName.toStdString().empty()) {
        return;
    }
    if (!destinationFileName.endsWith(".db")) {
        destinationFileName = destinationFileName + ".db";
    }

    QString successText{tr("Dictionary export succeeded!")};
    QString failureText{tr("Dictionary export failed.")};

    showProgressDialog(tr("Exporting dictionaries..."));

    _watcher = new QFutureWatcher<bool>{this};
    disconnect(_watcher, nullptr, nullptr, nullptr);
    connect(_watcher, &QFutureWatcher<bool>::finished, this, [=]() {
        _progressDialog->reset();
        exportDatabaseResult(_watcher->result(), successText, failureText);
    });

    QFuture<bool> future = QtConcurrent::run([=]() {
        SQLDatabaseManager manager;

        // Do not attempt to replace database with itself
        if (destinationFileName == manager.getDictionaryDatabasePath()) {
            return false;
        }

        if (QFile::exists(destinationFileName)) {
            QFile::remove(destinationFileName);
        }
        return QFile::copy(manager.getDictionaryDatabasePath(), destinationFileName);
    });
    _watcher->setFuture(future);
}

void AdvancedTab::exportUserDatabase(void)
{
    QFileDialog *_fileDialog = new QFileDialog{this};
    _fileDialog->setAcceptMode(QFileDialog::AcceptSave);

    QString destinationFileName
        = _fileDialog->getSaveFileName(this,
                                       tr("Path to save exported database"),
                                       QDir::homePath());
    if (destinationFileName.toStdString().empty()) {
        return;
    }
    if (!destinationFileName.endsWith(".db")) {
        destinationFileName = destinationFileName + ".db";
    }

    QString successText{tr("Saved words and history export succeeded!")};
    QString failureText{tr("Saved words and history export failed.")};

    showProgressDialog(tr("Exporting saved words and history..."));

    _watcher = new QFutureWatcher<bool>{this};
    disconnect(_watcher, nullptr, nullptr, nullptr);
    connect(_watcher, &QFutureWatcher<bool>::finished, this, [=]() {
        _progressDialog->reset();
        exportDatabaseResult(_watcher->result(), successText, failureText);
    });

    QFuture<bool> future = QtConcurrent::run([=]() {
        SQLDatabaseManager manager;

        // Do not attempt to replace database with itself
        if (destinationFileName == manager.getUserDatabasePath()) {
            return false;
        }

        if (QFile::exists(destinationFileName)) {
            QFile::remove(destinationFileName);
        }
        return QFile::copy(manager.getUserDatabasePath(), destinationFileName);
    });
    _watcher->setFuture(future);
}

void AdvancedTab::exportDatabaseResult(bool succeeded,
                                       const QString &suceededText,
                                       const QString &failedText)
{
    if (succeeded) {
        _exportDatabaseDialog = new ExportDatabaseDialog{suceededText, "", this};
    } else {
        _exportDatabaseDialog = new ExportDatabaseDialog{failedText, "", this};
    }

    _exportDatabaseDialog->setAttribute(Qt::WA_DeleteOnClose, true);
    _exportDatabaseDialog->exec();
}

void AdvancedTab::restoreBackedUpDictionaryDatabase(void)
{
    // Note: this function does not work in Windows. The OS prevents
    // the program from deleting the dictionary database file, since the file
    // is still in use by other instances of SQLDatabaseManager.
    QString successText{
        tr("Dictionary restore succeeded! %1 will now restart.")
            .arg(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                             Strings::PRODUCT_NAME))};
    QString failureText{
        tr("Dictionary restore failed! %1 will now restart.")
            .arg(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                             Strings::PRODUCT_NAME))};

    showProgressDialog(tr("Restoring dictionary..."));

    _watcher = new QFutureWatcher<bool>{this};
    disconnect(_watcher, nullptr, nullptr, nullptr);
    connect(_watcher, &QFutureWatcher<bool>::finished, this, [=]() {
        _progressDialog->reset();
        restoreDatabaseResult(_watcher->result(), successText, failureText);
    });

    QFuture<bool> future = QtConcurrent::run([]() {
        SQLDatabaseManager manager;
        return manager.restoreBackedUpDictionaryDatabase();
    });
    _watcher->setFuture(future);
}

void AdvancedTab::restoreExportedDictionaryDatabase(void)
{
    // Note: this function does not work in Windows. The OS prevents
    // the program from deleting the dictionary database file, since the file
    // is still in use by other instances of SQLDatabaseManager.
    QFileDialog *_fileDialog = new QFileDialog{this};
    _fileDialog->setAcceptMode(QFileDialog::AcceptOpen);

    QString sourceFileName
        = _fileDialog->getOpenFileName(this,
                                       tr("Exported dictionary file path"),
                                       QDir::homePath());
    if (sourceFileName.toStdString().empty()) {
        return;
    }

    QString successText{
        tr("Dictionary restore succeeded! %1 will now restart.")
            .arg(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                             Strings::PRODUCT_NAME))};

    QString failureText{
        tr("Dictionary restore failed! %1 will now restart.")
            .arg(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                             Strings::PRODUCT_NAME))};

    if (!QFile::exists(sourceFileName)) {
        restoreDatabaseResult(false, successText, failureText);
        return;
    }

    showProgressDialog(tr("Restoring dictionary..."));

    _watcher = new QFutureWatcher<bool>{this};
    disconnect(_watcher, nullptr, nullptr, nullptr);
    connect(_watcher, &QFutureWatcher<bool>::finished, this, [=]() {
        _progressDialog->reset();
        restoreDatabaseResult(_watcher->result(), successText, failureText);
    });

    auto future = QtConcurrent::run([=]() {
        SQLDatabaseManager manager;

        // Do not attempt to replace database with itself
        if (sourceFileName == manager.getDictionaryDatabasePath()) {
            return false;
        }

        manager.backupDictionaryDatabase();
        QFile::remove(manager.getDictionaryDatabasePath());
        return QFile::copy(sourceFileName, manager.getDictionaryDatabasePath());
    });
    _watcher->setFuture(future);
}

void AdvancedTab::restoreExportedUserDatabase(void)
{
    // Note: this function does not work in Windows. The OS prevents
    // the program from deleting the user database file, since the file
    // is still in use by other instances of SQLDatabaseManager.
    QFileDialog *_fileDialog = new QFileDialog{this};
    _fileDialog->setAcceptMode(QFileDialog::AcceptOpen);

    QString sourceFileName = _fileDialog->getOpenFileName(
        this, tr("Exported saved words and history path"), QDir::homePath());
    if (sourceFileName.toStdString().empty()) {
        return;
    }

    QString successText{
        tr("Saved words and history restore succeeded! %1 will now restart.")
            .arg(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                             Strings::PRODUCT_NAME))};
    QString failureText{
        tr("Saved words and history restore failed! %1 will now restart.")
            .arg(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                             Strings::PRODUCT_NAME))};

    if (!QFile::exists(sourceFileName)) {
        restoreDatabaseResult(false, successText, failureText);
        return;
    }

    showProgressDialog(tr("Restoring saved words and history..."));

    _watcher = new QFutureWatcher<bool>{this};
    disconnect(_watcher, nullptr, nullptr, nullptr);
    connect(_watcher, &QFutureWatcher<bool>::finished, this, [=]() {
        _progressDialog->reset();
        restoreDatabaseResult(_watcher->result(), successText, failureText);
    });

    auto future = QtConcurrent::run([=]() {
        SQLDatabaseManager manager;

        // Do not attempt to replace database with itself
        if (sourceFileName == manager.getUserDatabasePath()) {
            return false;
        }

        QFile::remove(manager.getUserDatabasePath());
        return QFile::copy(sourceFileName, manager.getUserDatabasePath());
    });
    _watcher->setFuture(future);
}

void AdvancedTab::restoreDatabaseResult(bool succeeded,
                                        const QString &suceededText,
                                        const QString &failedText)
{
    if (succeeded) {
        _restoreDatabaseDialog = new RestoreDatabaseDialog{suceededText,
                                                           "",
                                                           this};
    } else {
        _restoreDatabaseDialog = new RestoreDatabaseDialog{failedText, "", this};
    }

    _restoreDatabaseDialog->setAttribute(Qt::WA_DeleteOnClose, true);
    _restoreDatabaseDialog->exec();

    // Restart application
    qApp->quit();
    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
}

void AdvancedTab::showProgressDialog(QString text)
{
    _progressDialog = new QProgressDialog{"", QString(), 0, 0, this};
    _progressDialog->setWindowModality(Qt::ApplicationModal);
    _progressDialog->setMinimumSize(300, 75);
    Qt::WindowFlags flags = _progressDialog->windowFlags() | Qt::CustomizeWindowHint;
    flags &= ~(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint
               | Qt::WindowFullscreenButtonHint | Qt::WindowContextHelpButtonHint);
    _progressDialog->setWindowFlags(flags);
    _progressDialog->setMinimumDuration(500);
#ifdef Q_OS_WIN
    _progressDialog->setWindowTitle(
        QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::PRODUCT_NAME));
#elif defined(Q_OS_LINUX)
    _progressDialog->setWindowTitle(" ");
#endif
    _progressDialog->setAttribute(Qt::WA_DeleteOnClose, true);
    _progressDialog->setLabelText(text);
    _progressDialog->setRange(0, 0);
    _progressDialog->setValue(0);
}

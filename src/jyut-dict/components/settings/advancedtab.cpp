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
#include "logic/utils/utils_qt.h"

#include <KZip>

#include <QApplication>
#include <QFileDialog>
#include <QFrame>
#include <QLibraryInfo>
#include <QTimer>
#include <QtConcurrent/QtConcurrent>
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
#include <QWindow>
#endif

namespace {
constexpr auto AUDIO_DOWNLOAD_URL
    = "https://jyutdictionary.com/static/audio/v1/";
}

AdvancedTab::AdvancedTab(QWidget *parent)
    : QWidget{parent}
{
    setObjectName("AdvancedTab");

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
    _cantoneseTTSLayout = new QGridLayout{_cantoneseTTSWidget};
    _cantoneseTTSLayout->setContentsMargins(0, 0, 0, 0);
    _useCantoneseQtTTSBackend = new QRadioButton{this};
    _useCantoneseQtTTSBackend
        ->setProperty("data",
                      QVariant::fromValue(TextToSpeech::SpeakerBackend::QT_TTS));
    _useCantoneseGoogleOfflineSyllableTTSBackend = new QRadioButton{this};
    _useCantoneseGoogleOfflineSyllableTTSBackend->setProperty(
        "data",
        QVariant::fromValue(
            TextToSpeech::SpeakerBackend::GOOGLE_OFFLINE_SYLLABLE_TTS));
    initializeCantoneseTTSWidget(_cantoneseTTSWidget);

    _mandarinTTSWidget = new QWidget{this};
    _mandarinTTSLayout = new QGridLayout{_mandarinTTSWidget};
    _mandarinTTSLayout->setContentsMargins(0, 0, 0, 0);
    _useMandarinQtTTSBackend = new QRadioButton{this};
    _useMandarinQtTTSBackend
        ->setProperty("data",
                      QVariant::fromValue(TextToSpeech::SpeakerBackend::QT_TTS));
    _useMandarinGoogleOfflineSyllableTTSBackend = new QRadioButton{this};
    _useMandarinGoogleOfflineSyllableTTSBackend->setProperty(
        "data",
        QVariant::fromValue(
            TextToSpeech::SpeakerBackend::GOOGLE_OFFLINE_SYLLABLE_TTS));
    initializeMandarinTTSWidget(_mandarinTTSWidget);

    _ttsExplainer = new QLabel{this};

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
    _tabLayout->addRow(" ", _ttsExplainer);
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

    QColor backgroundColour = Utils::isDarkMode()
                                  ? QColor{LABEL_TEXT_COLOUR_DARK_R,
                                           LABEL_TEXT_COLOUR_DARK_G,
                                           LABEL_TEXT_COLOUR_DARK_B}
                                  : QColor{LABEL_TEXT_COLOUR_LIGHT_R,
                                           LABEL_TEXT_COLOUR_LIGHT_R,
                                           LABEL_TEXT_COLOUR_LIGHT_R};
    _ttsExplainer->setText(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                                       Strings::TTS_EXPLAINER)
                               .arg(backgroundColour.name()));

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
    if (!use_dark) {
#endif
        setAttribute(Qt::WA_StyledBackground);
        setStyleSheet("QWidget#AdvancedTab { "
                      "   background-color: palette(base);"
                      "} ");
#ifdef Q_OS_MAC
    }
#endif
}

void AdvancedTab::initializeUpdateCheckbox(QCheckBox &checkbox)
{
    connect(&checkbox, &QCheckBox::checkStateChanged, this, [&]() {
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

    connect(&checkbox, &QCheckBox::checkStateChanged, this, [&]() {
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

void AdvancedTab::initializeCantoneseTTSWidget(QWidget *widget)
{
    static_cast<QGridLayout *>(widget->layout())
        ->addWidget(_useCantoneseQtTTSBackend, 0, 0, 1, 1);
    static_cast<QGridLayout *>(widget->layout())
        ->addWidget(_useCantoneseGoogleOfflineSyllableTTSBackend, 0, 1, 1, 1);

    connect(_useCantoneseQtTTSBackend, &QRadioButton::clicked, this, [&]() {
        setCantoneseTTSSettings(TextToSpeech::SpeakerBackend::QT_TTS,
                                TextToSpeech::SpeakerVoice::NONE);
    });

    _cantoneseTTSCallbacks = std::make_shared<TextToSpeechCallbacks>(
        std::bind(&AdvancedTab::setCantoneseTTSWidgetDefault,
                  this,
                  _cantoneseTTSWidget),
        std::bind(&AdvancedTab::setCantoneseTTSSettings,
                  this,
                  TextToSpeech::SpeakerBackend::GOOGLE_OFFLINE_SYLLABLE_TTS,
                  TextToSpeech::SpeakerVoice::YUE_1)),

    connect(_useCantoneseGoogleOfflineSyllableTTSBackend,
            &QRadioButton::clicked,
            this,
            [&]() { startAudioDownload(_cantoneseTTSCallbacks); });

    setCantoneseTTSWidgetDefault(widget);
}

void AdvancedTab::initializeMandarinTTSWidget(QWidget *widget)
{
    static_cast<QGridLayout *>(widget->layout())
        ->addWidget(_useMandarinQtTTSBackend, 0, 0, 1, 1);
    static_cast<QGridLayout *>(widget->layout())
        ->addWidget(_useMandarinGoogleOfflineSyllableTTSBackend, 0, 1, 1, 1);

    connect(_useMandarinQtTTSBackend, &QRadioButton::clicked, this, [&]() {
        setMandarinTTSSettings(TextToSpeech::SpeakerBackend::QT_TTS,
                               TextToSpeech::SpeakerVoice::NONE);
    });

    _mandarinTTSCallbacks = std::make_shared<TextToSpeechCallbacks>(
        std::bind(&AdvancedTab::setMandarinTTSWidgetDefault,
                  this,
                  _mandarinTTSWidget),
        std::bind(&AdvancedTab::setMandarinTTSSettings,
                  this,
                  TextToSpeech::SpeakerBackend::GOOGLE_OFFLINE_SYLLABLE_TTS,
                  TextToSpeech::SpeakerVoice::CMN_1)),

    connect(_useMandarinGoogleOfflineSyllableTTSBackend,
            &QRadioButton::clicked,
            this,
            [&]() { startAudioDownload(_mandarinTTSCallbacks); });

    setMandarinTTSWidgetDefault(widget);
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
                (void) Settings::systemTranslator
                    .load("qt_" + newLocale.name(),
                          QLibraryInfo::path(QLibraryInfo::TranslationsPath));
                qApp->installTranslator(&Settings::systemTranslator);

                qApp->removeTranslator(&Settings::applicationTranslator);
                (void) Settings::applicationTranslator
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

void AdvancedTab::setCantoneseTTSWidgetDefault(QWidget *widget)
{
    TextToSpeech::SpeakerBackend backend
        = Settings::getSettings()
              ->value("Advanced/CantoneseTextToSpeech::SpeakerBackend",
                      QVariant::fromValue(TextToSpeech::SpeakerBackend::QT_TTS))
              .value<TextToSpeech::SpeakerBackend>();

    QList<QRadioButton *> buttons = widget->findChildren<QRadioButton *>();
    foreach (const auto &button, buttons) {
        if (button->property("data").isValid()
            && button->property("data").value<TextToSpeech::SpeakerBackend>()
                   == backend) {
            button->click();
#ifdef Q_OS_MAC
            // Makes the button selection show up correctly on macOS
            button->setDown(true);
#endif
        }
    }
}

void AdvancedTab::setCantoneseTTSSettings(TextToSpeech::SpeakerBackend backend,
                                          TextToSpeech::SpeakerVoice voice)
{
    _settings->setValue("Advanced/CantoneseTextToSpeech::SpeakerBackend",
                        QVariant::fromValue<TextToSpeech::SpeakerBackend>(
                            backend));
    _settings->setValue("Advanced/CantoneseTextToSpeech::SpeakerVoice",
                        QVariant::fromValue<TextToSpeech::SpeakerVoice>(voice));
    _settings->sync();
}

void AdvancedTab::setMandarinTTSWidgetDefault(QWidget *widget)
{
    TextToSpeech::SpeakerBackend backend
        = Settings::getSettings()
              ->value("Advanced/MandarinTextToSpeech::SpeakerBackend",
                      QVariant::fromValue(TextToSpeech::SpeakerBackend::QT_TTS))
              .value<TextToSpeech::SpeakerBackend>();

    QList<QRadioButton *> buttons = widget->findChildren<QRadioButton *>();
    foreach (const auto &button, buttons) {
        if (button->property("data").isValid()
            && button->property("data").value<TextToSpeech::SpeakerBackend>()
                   == backend) {
            button->click();
#ifdef Q_OS_MAC
            // Makes the button selection show up correctly on macOS
            button->setDown(true);
#endif
        }
    }
}

void AdvancedTab::setMandarinTTSSettings(TextToSpeech::SpeakerBackend backend,
                                         TextToSpeech::SpeakerVoice voice)
{
    _settings->setValue("Advanced/MandarinTextToSpeech::SpeakerBackend",
                        QVariant::fromValue<TextToSpeech::SpeakerBackend>(
                            backend));
    _settings->setValue("Advanced/MandarinTextToSpeech::SpeakerVoice",
                        QVariant::fromValue<TextToSpeech::SpeakerVoice>(voice));
    _settings->sync();
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
    setCantoneseTTSWidgetDefault(_cantoneseTTSWidget);
    setMandarinTTSWidgetDefault(_cantoneseTTSWidget);
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

    _boolReturnWatcher = new QFutureWatcher<bool>{this};
    disconnect(_boolReturnWatcher, nullptr, nullptr, nullptr);
    connect(_boolReturnWatcher,
            &QFutureWatcher<bool>::finished,
            this,
            [=, this]() {
                _progressDialog->reset();
                exportDatabaseResult(_boolReturnWatcher->result(),
                                     successText,
                                     failureText);
            });
    QFuture<bool> future = QtConcurrent::run([=, this]() {
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
    _boolReturnWatcher->setFuture(future);
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

    _boolReturnWatcher = new QFutureWatcher<bool>{this};
    disconnect(_boolReturnWatcher, nullptr, nullptr, nullptr);
    connect(_boolReturnWatcher,
            &QFutureWatcher<bool>::finished,
            this,
            [=, this]() {
                _progressDialog->reset();
                exportDatabaseResult(_boolReturnWatcher->result(),
                                     successText,
                                     failureText);
            });
    QFuture<bool> future = QtConcurrent::run([=, this]() {
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
    _boolReturnWatcher->setFuture(future);
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

    _boolReturnWatcher = new QFutureWatcher<bool>{this};
    disconnect(_boolReturnWatcher, nullptr, nullptr, nullptr);
    connect(_boolReturnWatcher,
            &QFutureWatcher<bool>::finished,
            this,
            [=, this]() {
                _progressDialog->reset();
                restoreDatabaseResult(_boolReturnWatcher->result(),
                                      successText,
                                      failureText);
            });
    QFuture<bool> future = QtConcurrent::run([]() {
        SQLDatabaseManager manager;
        return manager.restoreBackedUpDictionaryDatabase();
    });
    _boolReturnWatcher->setFuture(future);
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

    _boolReturnWatcher = new QFutureWatcher<bool>{this};
    disconnect(_boolReturnWatcher, nullptr, nullptr, nullptr);
    connect(_boolReturnWatcher,
            &QFutureWatcher<bool>::finished,
            this,
            [=, this]() {
                _progressDialog->reset();
                restoreDatabaseResult(_boolReturnWatcher->result(),
                                      successText,
                                      failureText);
            });
    auto future = QtConcurrent::run([=, this]() {
        SQLDatabaseManager manager;

        // Do not attempt to replace database with itself
        if (sourceFileName == manager.getDictionaryDatabasePath()) {
            return false;
        }

        manager.backupDictionaryDatabase();
        QFile::remove(manager.getDictionaryDatabasePath());
        return QFile::copy(sourceFileName, manager.getDictionaryDatabasePath());
    });
    _boolReturnWatcher->setFuture(future);
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

    _boolReturnWatcher = new QFutureWatcher<bool>{this};
    disconnect(_boolReturnWatcher, nullptr, nullptr, nullptr);
    connect(_boolReturnWatcher,
            &QFutureWatcher<bool>::finished,
            this,
            [=, this]() {
                _progressDialog->reset();
                restoreDatabaseResult(_boolReturnWatcher->result(),
                                      successText,
                                      failureText);
            });
    auto future = QtConcurrent::run([=, this]() {
        SQLDatabaseManager manager;

        // Do not attempt to replace database with itself
        if (sourceFileName == manager.getUserDatabasePath()) {
            return false;
        }

        QFile::remove(manager.getUserDatabasePath());
        return QFile::copy(sourceFileName, manager.getUserDatabasePath());
    });
    _boolReturnWatcher->setFuture(future);
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

void AdvancedTab::startAudioDownload(std::shared_ptr<TextToSpeechCallbacks> cbs)
{
    if (!sender()) {
        return;
    }

    auto backend
        = sender()->property("data").value<TextToSpeech::SpeakerBackend>();
    if (QDir{EntrySpeaker::getAudioPath() + TextToSpeech::backendNames[backend]}
            .exists()) {
        // In the future, there should be logic here that tests whether
        // the downloaded version is the newest.
        cbs->successCb();
        return;
    }

    _downloadAudioDialog = new DownloadAudioDialog{this};
    if (_downloadAudioDialog->exec() != QMessageBox::Yes) {
        cbs->resetCb();
        return;
    }

    showProgressDialog(tr("Downloading audio files..."));

    QString zipFile
        = QStandardPaths::standardLocations(QStandardPaths::TempLocation).at(0)
          + "/" + TextToSpeech::backendNames[backend];
    QUrl url{AUDIO_DOWNLOAD_URL + TextToSpeech::backendNames[backend] + ".zip"};

    _downloader = new Downloader(url, zipFile, this);

    disconnect(_downloader, nullptr, nullptr, nullptr);
    connect(_downloader, &Downloader::downloaded, this, [=, this](QString outputPath) {
        // Since the std::shared_ptr cbs goes out of scope once this function ends,
        // it must be captured by value instead of by reference
        unzipFile(outputPath, cbs);
    });
    connect(_downloader, &Downloader::error, this, [=, this](int error) {
        _progressDialog->reset();
        downloadAudioResult(!error,
                            tr("Audio downloaded successfully!"),
                            tr("Audio could not be downloaded, error code %1.")
                                .arg(error));
        if (error) {
            // An error happened, set it back to original backend
            cbs->resetCb();
        }
    });

    _downloader->startDownload();
}

void AdvancedTab::downloadAudioResult(bool succeeded,
                                      const QString &suceededText,
                                      const QString &failedText)
{
    if (succeeded) {
        _downloadResultDialog = new DownloadResultDialog{suceededText, "", this};
    } else {
        _downloadResultDialog = new DownloadResultDialog{failedText, "", this};
    }

    _downloadResultDialog->setAttribute(Qt::WA_DeleteOnClose, true);
    _downloadResultDialog->exec();
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

void AdvancedTab::unzipFile(QString outputPath,
                            std::shared_ptr<TextToSpeechCallbacks> cbs)
{
    QString outputFolder = EntrySpeaker::getAudioPath();
    _progressDialog->setLabelText(tr("Installing downloaded files..."));

    _boolReturnWatcher = new QFutureWatcher<bool>{this};
    QFuture<bool> future = QtConcurrent::run([=, this]() {
        KZip zip{outputPath};
        if (!zip.open(QIODevice::ReadOnly)) {
            return false;
        }
        if (!zip.directory()->copyTo(outputFolder)) {
            return false;
        }
        return true;
    });
    _boolReturnWatcher->setFuture(future);
    connect(_boolReturnWatcher, &QFutureWatcher<bool>::finished, this, [=, this]() {
        // Since the std::shared_ptr cbs goes out of scope once this function ends,
        // it must be captured by value instead of by reference
        unzipComplete(static_cast<QFutureWatcher<bool> *>(sender())->result(),
                      cbs);
    });
}

void AdvancedTab::unzipComplete(bool completed,
                                std::shared_ptr<TextToSpeechCallbacks> cbs)
{
    if (completed) {
        cbs->successCb();
    } else {
        cbs->resetCb();
    }
    _progressDialog->reset();
    downloadAudioResult(completed,
                        tr("Files installed successfully!"),
                        tr("Files could not be installed."));
}

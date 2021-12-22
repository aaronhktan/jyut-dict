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

    QFrame *_languageDivider = new QFrame{this};
    _languageDivider->setObjectName("divider");
    _languageDivider->setFrameShape(QFrame::HLine);
    _languageDivider->setFrameShadow(QFrame::Raised);
    _languageDivider->setFixedHeight(1);

    _languageCombobox = new QComboBox{this};
    _languageCombobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    initializeLanguageCombobox(*_languageCombobox);

    _tabLayout->addRow(" ", _updateCheckbox);
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    _tabLayout->addRow(" ", _forceDarkModeCheckbox);
#endif
    _tabLayout->addRow(_exportDivider);
    _tabLayout->addRow(" ", _exportDictionaryDatabaseButton);
    _tabLayout->addRow(" ", _exportUserDatabaseButton);
    _tabLayout->addRow(_restoreDivider);
    _tabLayout->addRow(" ", _restoreBackedUpDictionaryDatabaseButton);
    _tabLayout->addRow(" ", _restoreExportedDictionaryDatabaseButton);
    _tabLayout->addRow(" ", _restoreExportedUserDatabaseButton);
    _tabLayout->addRow(_languageDivider);
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
    static_cast<QLabel *>(_tabLayout->labelForField(_exportDictionaryDatabaseButton))
        ->setText(tr("Export dictionaries file:"));
    static_cast<QLabel *>(_tabLayout->labelForField(_exportUserDatabaseButton))
        ->setText(tr("Export saved words and history:"));
    _exportDictionaryDatabaseButton->setText(tr("Export"));
    _exportUserDatabaseButton->setText(tr("Export"));

    static_cast<QLabel *>(_tabLayout->labelForField(_restoreBackedUpDictionaryDatabaseButton))
        ->setText(tr("Restore dictionaries file to last backed up version:"));
    static_cast<QLabel *>(_tabLayout->labelForField(_restoreExportedDictionaryDatabaseButton))
        ->setText(tr("Restore dictionaries file using exported file:"));
    static_cast<QLabel *>(_tabLayout->labelForField(_restoreExportedUserDatabaseButton))
        ->setText(tr("Restore saved words and history file using exported file:"));
    _restoreBackedUpDictionaryDatabaseButton->setText(tr("Restore"));
    _restoreExportedDictionaryDatabaseButton->setText(tr("Restore"));
    _restoreExportedUserDatabaseButton->setText(tr("Restore"));


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
        if (QFile::exists(destinationFileName)) {
            QFile::remove(destinationFileName);
        }
        SQLDatabaseManager manager;
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
        if (QFile::exists(destinationFileName)) {
            QFile::remove(destinationFileName);
        }
        SQLDatabaseManager manager;
        return QFile::copy(manager.getUserDatabasePath(), destinationFileName);
    });
    _watcher->setFuture(future);
}

void AdvancedTab::exportDatabaseResult(bool succeeded,
                                       QString suceededText,
                                       QString failedText)
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
    QString successText{tr("Dictionary restore succeeded! Jyut "
                           "Dictionary will now restart.")};
    QString failureText{
        tr("Dictionary restore failed! Jyut Dictionary will now restart.")};

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
    QFileDialog *_fileDialog = new QFileDialog{this};
    _fileDialog->setAcceptMode(QFileDialog::AcceptOpen);

    QString sourceFileName
        = _fileDialog->getOpenFileName(this,
                                       tr("Exported dictionary file path"),
                                       QDir::homePath());
    if (sourceFileName.toStdString().empty()) {
        return;
    }

    QString successText{tr("Dictionary restore succeeded! Jyut "
                           "Dictionary will now restart.")};
    QString failureText{
        tr("Dictionary restore failed! Jyut Dictionary will now restart.")};

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
        manager.backupDictionaryDatabase();
        QFile::remove(manager.getDictionaryDatabasePath());
        return QFile::copy(sourceFileName, manager.getDictionaryDatabasePath());
    });
    _watcher->setFuture(future);
}

void AdvancedTab::restoreExportedUserDatabase(void)
{
    QFileDialog *_fileDialog = new QFileDialog{this};
    _fileDialog->setAcceptMode(QFileDialog::AcceptOpen);

    QString sourceFileName = _fileDialog->getOpenFileName(
        this, tr("Exported saved words and history path"), QDir::homePath());
    if (sourceFileName.toStdString().empty()) {
        return;
    }

    QString successText{tr("Saved words and history restore succeeded! Jyut "
                           "Dictionary will now restart.")};
    QString failureText{tr("Saved words and history restore "
                           "failed! Jyut Dictionary will now "
                           "restart.")};

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
        QFile::remove(manager.getUserDatabasePath());
        return QFile::copy(sourceFileName, manager.getUserDatabasePath());
    });
    _watcher->setFuture(future);
}

void AdvancedTab::restoreDatabaseResult(bool succeeded,
                                        QString suceededText,
                                        QString failedText)
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

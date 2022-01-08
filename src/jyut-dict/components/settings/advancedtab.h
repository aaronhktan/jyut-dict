#ifndef ADVANCEDTAB_H
#define ADVANCEDTAB_H

#include "dialogs/exportdatabasedialog.h"
#include "dialogs/restoredatabasedialog.h"
#include "logic/settings/settingsutils.h"

#include <QCheckBox>
#include <QComboBox>
#include <QEvent>
#include <QFormLayout>
#include <QFutureWatcher>
#include <QLabel>
#include <QProgressDialog>
#include <QPushButton>
#include <QSettings>
#include <QTranslator>
#include <QWidget>

// The advanced tab displays advanced options in the settings menu.

class AdvancedTab : public QWidget
{
    Q_OBJECT

public:
    explicit AdvancedTab(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

private:
    void setupUI();
    void translateUI();

    void setStyle(bool use_dark);

    void initializeUpdateCheckbox(QCheckBox &checkbox);
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    void initializeForceDarkModeCheckbox(QCheckBox &checkbox);
#endif
    void initializeLanguageCombobox(QComboBox &combobox);

    void exportDictionaryDatabase(void);
    void exportUserDatabase(void);

    void exportDatabaseResult(bool succeeded,
                              QString suceededText,
                              QString failedText);

    void restoreBackedUpDictionaryDatabase(void);
    void restoreExportedDictionaryDatabase(void);
    void restoreExportedUserDatabase(void);

    void restoreDatabaseResult(bool succeeded,
                               QString suceededText,
                               QString failedText);

    void showProgressDialog(QString text);

    bool _paletteRecentlyChanged = false;

    QCheckBox *_updateCheckbox;
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    QCheckBox *_forceDarkModeCheckbox;
#endif
    QPushButton *_exportDictionaryDatabaseButton;
    QPushButton *_exportUserDatabaseButton;
    QPushButton *_restoreBackedUpDictionaryDatabaseButton;
    QPushButton *_restoreExportedDictionaryDatabaseButton;
    QPushButton *_restoreExportedUserDatabaseButton;
    QComboBox *_languageCombobox;

    QFormLayout *_tabLayout;

    std::unique_ptr<QSettings> _settings;

    QFutureWatcher<bool> *_watcher;

    QProgressDialog *_progressDialog;
    ExportDatabaseDialog *_exportDatabaseDialog;
    RestoreDatabaseDialog *_restoreDatabaseDialog;
};

#endif // ADVANCEDTAB_H

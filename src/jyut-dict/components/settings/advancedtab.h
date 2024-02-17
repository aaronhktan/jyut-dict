#ifndef ADVANCEDTAB_H
#define ADVANCEDTAB_H

#include "dialogs/exportdatabasedialog.h"
#include "dialogs/restoredatabasedialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QEvent>
#include <QFormLayout>
#include <QFutureWatcher>
#include <QLabel>
#include <QProgressDialog>
#include <QPushButton>
#include <QRadioButton>
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
    void initializeCantoneseTTSWidget(QWidget &widget);
    void initializeMandarinTTSWidget(QWidget &widget);
    void initializeLanguageCombobox(QComboBox &combobox);
    void initializeResetButton(QPushButton &resetButton);

    void setUpdateCheckboxDefault(QCheckBox &checkbox);
    void setLanguageComboboxDefault(QComboBox &combobox);
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    void setForceDarkModeCheckboxDefault(QCheckBox &checkbox);
#endif
    void setCantoneseTTSWidgetDefault(QWidget &widget);
    void setMandarinTTSWidgetDefault(QWidget &widget);

    void resetSettings(QSettings &settings);

    void exportDictionaryDatabase(void);
    void exportUserDatabase(void);

    void exportDatabaseResult(bool succeeded,
                              const QString &suceededText,
                              const QString &failedText);

    void restoreBackedUpDictionaryDatabase(void);
    void restoreExportedDictionaryDatabase(void);
    void restoreExportedUserDatabase(void);

    void restoreDatabaseResult(bool succeeded,
                               const QString &suceededText,
                               const QString &failedText);

    void showProgressDialog(QString text);

    bool _paletteRecentlyChanged = false;

    QCheckBox *_updateCheckbox;
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    QCheckBox *_forceDarkModeCheckbox;
#endif

    QWidget *_cantoneseTTSWidget;
    QLayout *_cantoneseTTSLayout;
    QRadioButton *_useCantoneseQtTTSBackend;
    QRadioButton *_useCantoneseGoogleOfflineSyllableTTSBackend;
    QRadioButton *_useYue1Voice;
    QRadioButton *_useYue2Voice;

    QWidget *_mandarinTTSWidget;
    QLayout *_mandarinTTSLayout;
    QRadioButton *_useMandarinQtTTSBackend;
    QRadioButton *_useMandarinGoogleOfflineSyllableTTSBackend;
    QRadioButton *_useCmn1Voice;
    QRadioButton *_useCmn2Voice;

    QPushButton *_exportDictionaryDatabaseButton;
    QPushButton *_exportUserDatabaseButton;
    QPushButton *_restoreBackedUpDictionaryDatabaseButton;
    QPushButton *_restoreExportedDictionaryDatabaseButton;
    QPushButton *_restoreExportedUserDatabaseButton;
    QComboBox *_languageCombobox;
    QPushButton *_resetButton;

    QFormLayout *_tabLayout;

    std::unique_ptr<QSettings> _settings;

    QFutureWatcher<bool> *_watcher;

    QProgressDialog *_progressDialog;
    ExportDatabaseDialog *_exportDatabaseDialog;
    RestoreDatabaseDialog *_restoreDatabaseDialog;

signals:
    void settingsReset(void);
};

#endif // ADVANCEDTAB_H

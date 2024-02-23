#ifndef ADVANCEDTAB_H
#define ADVANCEDTAB_H

#include "dialogs/downloadaudiodialog.h"
#include "dialogs/downloadresultdialog.h"
#include "dialogs/exportdatabasedialog.h"
#include "dialogs/restoredatabasedialog.h"
#include "logic/download/downloader.h"
#include "logic/entry/entryspeaker.h"

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

#include <functional>

// The advanced tab displays advanced options in the settings window.

class AdvancedTab : public QWidget
{
    Q_OBJECT

public:
    explicit AdvancedTab(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

private:
    struct TextToSpeechCallbacks
    {
        TextToSpeechCallbacks(std::function<void()> reset,
                              std::function<void()> success)
            : resetCb{reset}
            , successCb{success}
        {}

        std::function<void()> resetCb = nullptr;
        std::function<void()> successCb = nullptr;
    };

    void setupUI();
    void translateUI();

    void setStyle(bool use_dark);

    void initializeUpdateCheckbox(QCheckBox &checkbox);
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    void initializeForceDarkModeCheckbox(QCheckBox &checkbox);
#endif
    void initializeCantoneseTTSWidget(QWidget *widget);
    void initializeMandarinTTSWidget(QWidget *widget);
    void initializeLanguageCombobox(QComboBox &combobox);
    void initializeResetButton(QPushButton &resetButton);

    void setUpdateCheckboxDefault(QCheckBox &checkbox);
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    void setForceDarkModeCheckboxDefault(QCheckBox &checkbox);
#endif
    void setCantoneseTTSWidgetDefault(QWidget *widget);
    void setCantoneseTTSSettings(TextToSpeech::SpeakerBackend backend,
                                 TextToSpeech::SpeakerVoice voice);
    void setMandarinTTSWidgetDefault(QWidget *widget);
    void setMandarinTTSSettings(TextToSpeech::SpeakerBackend backend,
                                TextToSpeech::SpeakerVoice voice);
    void setLanguageComboboxDefault(QComboBox &combobox);

    void exportDictionaryDatabase(void);
    void exportUserDatabase(void);

    void exportDatabaseResult(bool succeeded,
                              const QString &suceededText,
                              const QString &failedText);

    void restoreBackedUpDictionaryDatabase(void);
    void restoreExportedDictionaryDatabase(void);
    void restoreExportedUserDatabase(void);

    void resetSettings(QSettings &settings);

    void restoreDatabaseResult(bool succeeded,
                               const QString &suceededText,
                               const QString &failedText);
    void startAudioDownload(std::shared_ptr<TextToSpeechCallbacks> cbs);
    void downloadAudioResult(bool succeeded,
                             const QString &succeededText,
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
    std::shared_ptr<TextToSpeechCallbacks> _cantoneseTTSCallbacks;

    QWidget *_mandarinTTSWidget;
    QLayout *_mandarinTTSLayout;
    QRadioButton *_useMandarinQtTTSBackend;
    QRadioButton *_useMandarinGoogleOfflineSyllableTTSBackend;
    std::shared_ptr<TextToSpeechCallbacks> _mandarinTTSCallbacks;

    QLabel *_ttsExplainer;

    QPushButton *_exportDictionaryDatabaseButton;
    QPushButton *_exportUserDatabaseButton;
    QPushButton *_restoreBackedUpDictionaryDatabaseButton;
    QPushButton *_restoreExportedDictionaryDatabaseButton;
    QPushButton *_restoreExportedUserDatabaseButton;
    QComboBox *_languageCombobox;
    QPushButton *_resetButton;

    QFormLayout *_tabLayout;

    std::unique_ptr<QSettings> _settings;

    QFutureWatcher<bool> *_boolReturnWatcher;

    QProgressDialog *_progressDialog;
    DownloadResultDialog *_downloadResultDialog;
    DownloadAudioDialog *_downloadAudioDialog;
    ExportDatabaseDialog *_exportDatabaseDialog;
    RestoreDatabaseDialog *_restoreDatabaseDialog;

    Downloader *_downloader;

private slots:
    void unzipFile(QString outputPath,
                   std::shared_ptr<TextToSpeechCallbacks> cbs);
    void unzipComplete(bool completed,
                       std::shared_ptr<TextToSpeechCallbacks> cbs);

signals:
    void settingsReset(void);
};

#endif // ADVANCEDTAB_H

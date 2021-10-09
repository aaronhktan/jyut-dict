#ifndef ADVANCEDTAB_H
#define ADVANCEDTAB_H

#include "logic/settings/settingsutils.h"

#include <QCheckBox>
#include <QComboBox>
#include <QEvent>
#include <QFormLayout>
#include <QLabel>
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

    void exportUserDatabase(void);

    bool _paletteRecentlyChanged = false;

    QCheckBox *_updateCheckbox;
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    QCheckBox *_forceDarkModeCheckbox;
#endif
    QPushButton *_exportUserDatabaseButton;
    QComboBox *_languageCombobox;

    QFormLayout *_tabLayout;

    std::unique_ptr<QSettings> _settings;
};

#endif // ADVANCEDTAB_H

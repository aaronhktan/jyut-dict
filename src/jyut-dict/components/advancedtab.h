#ifndef ADVANCEDTAB_H
#define ADVANCEDTAB_H

#include "logic/analytics/analytics.h"
#include "logic/settings/settingsutils.h"

#include <QCheckBox>
#include <QComboBox>
#include <QEvent>
#include <QFormLayout>
#include <QLabel>
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
    void initializeAnalyticsCheckbox(QCheckBox &checkbox);
    void initializeLanguageCombobox(QComboBox &combobox);

    bool _paletteRecentlyChanged = false;

    Analytics *_analytics;

    QCheckBox *_updateCheckbox;
    QCheckBox *_analyticsCheckbox;
    QComboBox *_languageCombobox;

    QFormLayout *_tabLayout;

    std::unique_ptr<QSettings> _settings;
};

#endif // ADVANCEDTAB_H

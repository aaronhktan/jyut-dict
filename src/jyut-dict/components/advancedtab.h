#ifndef ADVANCEDTAB_H
#define ADVANCEDTAB_H

#include "logic/settings/settingsutils.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QSettings>
#include <QWidget>

// The advanced tab displays advanced options in the settings menu.

class AdvancedTab : public QWidget
{
    Q_OBJECT

public:
    explicit AdvancedTab(QWidget *parent = nullptr);

private:
    void setupUI();
    void initializeUpdateCheckbox(QCheckBox &checkbox);

    QCheckBox *_updateCheckbox;

    QFormLayout *_tabLayout;

    std::unique_ptr<QSettings> _settings;
};

#endif // ADVANCEDTAB_H

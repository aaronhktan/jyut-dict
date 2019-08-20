#include "advancedtab.h"

AdvancedTab::AdvancedTab(QWidget *parent)
    : QWidget(parent)
{
    _settings = Settings::getSettings();
    setupUI();
}

void AdvancedTab::setupUI()
{
    _tabLayout = new QFormLayout{this};
#ifdef Q_OS_MAC
    _tabLayout->setFormAlignment(Qt::AlignHCenter | Qt::AlignCenter);
    _tabLayout->setContentsMargins(20, 20, 30, 20);
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

    _tabLayout->addRow(tr("Automatically check for updates on startup:"),
                       _updateCheckbox);
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


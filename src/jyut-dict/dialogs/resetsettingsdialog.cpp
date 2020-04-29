#include "resetsettingsdialog.h"

#include "logic/utils/utils.h"

ResetSettingsDialog::ResetSettingsDialog(QWidget *parent)
    : DefaultDialog("", "", parent)
{
    setupUI();
    translateUI();
}

void ResetSettingsDialog::setupUI(void)
{
    setText(tr("Are you sure you want to reset all settings?"));
    setInformativeText(tr("There is no way to restore them!"));
    setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    setWidth(400);
}

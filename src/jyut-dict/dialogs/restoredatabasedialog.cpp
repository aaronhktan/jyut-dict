#include "restoredatabasedialog.h"

RestoreDatabaseDialog::RestoreDatabaseDialog(const QString &reason,
                                             const QString &description,
                                             QWidget *parent)
    : DefaultDialog(reason, description, parent)
{
    setupUI();
    translateUI();
}

void RestoreDatabaseDialog::setupUI()
{
    setText(tr("Database restore complete!"));

    addButton(tr("OK"), QMessageBox::AcceptRole);

    setWidth(400);
    deselectButtons();
}

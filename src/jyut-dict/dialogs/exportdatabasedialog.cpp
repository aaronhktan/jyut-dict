#include "exportdatabasedialog.h"

ExportDatabaseDialog::ExportDatabaseDialog(const QString &reason,
                                            const QString &description,
                                           QWidget *parent)
    : DefaultDialog(reason, description, parent)
{
    setupUI();
    translateUI();
}

void ExportDatabaseDialog::setupUI()
{
    setText(tr("Database export complete!"));

    addButton(tr("OK"), QMessageBox::AcceptRole);

    setWidth(400);
    deselectButtons();
}

#include "downloadresultdialog.h"

DownloadResultDialog::DownloadResultDialog(const QString &reason,
                                           const QString &description,
                                           QWidget *parent)
    : DefaultDialog(reason, description, parent)
{
    setupUI();
    translateUI();
}

void DownloadResultDialog::setupUI()
{
    setText(tr("Your download has finished."));

    addButton(tr("OK"), QMessageBox::AcceptRole);

    setWidth(400);
    deselectButtons();
}

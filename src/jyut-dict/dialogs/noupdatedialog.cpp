#include "noupdatedialog.h"

#include "logic/utils/utils.h"

NoUpdateDialog::NoUpdateDialog(QString &version, QWidget *parent)
    : DefaultDialog("", "", parent)
{
    setText(tr("No update found!"));
    setInformativeText(tr("You are on the newest version, %1.").arg(version));
    setIcon(QMessageBox::Information);
    setStandardButtons(QMessageBox::Ok);
    setWidth(400);
}

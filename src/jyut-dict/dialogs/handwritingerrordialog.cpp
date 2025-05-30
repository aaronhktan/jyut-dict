#include "handwritingerrordialog.h"

#include <QAbstractButton>
#include <QDesktopServices>
#include <QPushButton>
#include <QUrl>

HandwritingErrorDialog::HandwritingErrorDialog(const QString &reason,
                                               const QString &description,
                                               QWidget *parent)
    : DefaultDialog(reason, description, parent)
{
    setupUI();
    translateUI();
}

void HandwritingErrorDialog::setupUI()
{
    setText(tr("Handwriting could not be started!"));
    addButton(tr("OK"), QMessageBox::AcceptRole);

    setWidth(400);
    deselectButtons();
}

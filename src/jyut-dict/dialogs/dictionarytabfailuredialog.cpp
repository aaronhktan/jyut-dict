#include "dictionarytabfailuredialog.h"

#include "logic/utils/utils.h"

DictionaryTabFailureDialog::DictionaryTabFailureDialog(const QString &reason,
                                             const QString &description,
                                             QWidget *parent)
    : DefaultDialog(reason, description, parent)
{
    setupUI();
    translateUI();
}

void DictionaryTabFailureDialog::setupUI(void)
{
    setText(tr("Failed to add source!"));
    addButton(tr("OK"), QMessageBox::AcceptRole);
    setWidth(400);
}

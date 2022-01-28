#include "overwriteconflictingdictionarydialog.h"

OverwriteConflictingDictionaryDialog::OverwriteConflictingDictionaryDialog(
    conflictingDictionaryMetadata conflictingDictionaries, QWidget *parent)
    : DefaultDialog{"", "", parent}
{
    setupUI(conflictingDictionaries);
    translateUI();
}

void OverwriteConflictingDictionaryDialog::setupUI(
    conflictingDictionaryMetadata conflictingDictionaries)
{
    setText(tr("Would you like to overwrite dictionaries you already have?"));
    QStringList dictionariesList;
    for (const auto &dictionary : conflictingDictionaries) {
        dictionariesList.append(std::get<0>(dictionary).c_str());
    }
    setInformativeText(tr("The dictionaries to be overwritten are:\n    - %1")
                           .arg(dictionariesList.join("\n    - ")));
    setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    setWidth(400);
}

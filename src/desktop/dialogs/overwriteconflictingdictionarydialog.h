#ifndef OVERWRITECONFLICTINGDICTIONARYDIALOG_H
#define OVERWRITECONFLICTINGDICTIONARYDIALOG_H

#include "dialogs/defaultdialog.h"

#include "logic/database/sqldatabaseutils.h"

class OverwriteConflictingDictionaryDialog : public DefaultDialog
{
    Q_OBJECT
public:
    explicit OverwriteConflictingDictionaryDialog(
        conflictingSourceMetadata conflictingDictionaries,
        QWidget *parent = nullptr);

private:
    void setupUI(conflictingSourceMetadata conflictingDictionaries);
};

#endif // OVERWRITECONFLICTINGDICTIONARYDIALOG_H

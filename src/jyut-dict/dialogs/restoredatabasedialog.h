#ifndef RESTOREDATABASEDIALOG_H
#define RESTOREDATABASEDIALOG_H

#include "dialogs/defaultdialog.h"

#include <QEvent>
#include <QString>

class RestoreDatabaseDialog : public DefaultDialog
{
    Q_OBJECT
public:
    explicit RestoreDatabaseDialog(const QString &reason,
                                   const QString &description,
                                   QWidget *parent = nullptr);

private:
    void setupUI(void);
};

#endif // RESTOREDATABASEDIALOG_H

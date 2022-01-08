#ifndef EXPORTDATABASEDIALOG_H
#define EXPORTDATABASEDIALOG_H

#include "dialogs/defaultdialog.h"

#include <QEvent>
#include <QString>

class ExportDatabaseDialog : public DefaultDialog
{
    Q_OBJECT
public:
    explicit ExportDatabaseDialog(const QString &reason,
                                  const QString &description,
                                  QWidget *parent = nullptr);

private:
    void setupUI(void);
};

#endif // EXPORTDATABASEDIALOG_H

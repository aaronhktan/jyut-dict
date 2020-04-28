#ifndef NOUPDATEDIALOG_H
#define NOUPDATEDIALOG_H

#include "dialogs/defaultdialog.h"

#include <QString>

class NoUpdateDialog : public DefaultDialog
{
    Q_OBJECT
public:
    explicit NoUpdateDialog(QString &version, QWidget *parent = nullptr);
};

#endif // NOUPDATEDIALOG_H

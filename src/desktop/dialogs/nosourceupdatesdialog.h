#ifndef NOSOURCEUPDATESDIALOG_H
#define NOSOURCEUPDATESDIALOG_H

#include "dialogs/defaultdialog.h"

class NoSourceUpdatesDialog : public DefaultDialog
{
    Q_OBJECT
public:
    explicit NoSourceUpdatesDialog(QWidget *parent = nullptr);

private:
    void setupUI();
};

#endif // NOSOURCEUPDATESDIALOG_H

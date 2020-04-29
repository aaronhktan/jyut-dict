#ifndef RESETSETTINGSDIALOG_H
#define RESETSETTINGSDIALOG_H

#include "dialogs/defaultdialog.h"

class ResetSettingsDialog : public DefaultDialog
{
    Q_OBJECT
public:
    explicit ResetSettingsDialog(QWidget *parent = nullptr);

private:
    void setupUI(void);
};

#endif // RESETSETTINGSDIALOG_H

#ifndef ENTRYSPEAKERRORDIALOG_H
#define ENTRYSPEAKERRORDIALOG_H

#include "dialogs/defaultdialog.h"

#include <QString>

class EntrySpeakErrorDialog : public DefaultDialog
{
    Q_OBJECT
public:
    explicit EntrySpeakErrorDialog(const QString &reason,
                                   const QString &description,
                                   QWidget *parent = nullptr);
};

#endif // ENTRYSPEAKERRORDIALOG_H

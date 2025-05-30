#ifndef HANDWRITINGERRORDIALOG_H
#define HANDWRITINGERRORDIALOG_H

#include "dialogs/defaultdialog.h"

#include <QEvent>
#include <QString>

class HandwritingErrorDialog : public DefaultDialog
{
    Q_OBJECT
public:
    explicit HandwritingErrorDialog(const QString &reason,
                                    const QString &description,
                                    QWidget *parent = nullptr);

private:
    void setupUI(void);
};

#endif // HANDWRITINGERRORDIALOG_H

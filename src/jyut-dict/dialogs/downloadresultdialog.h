#ifndef DOWNLOADRESULTDIALOG_H
#define DOWNLOADRESULTDIALOG_H

#include "dialogs/defaultdialog.h"

#include <QEvent>
#include <QString>

class DownloadResultDialog : public DefaultDialog
{
    Q_OBJECT
public:
    explicit DownloadResultDialog(const QString &reason,
                                  const QString &description,
                                  QWidget *parent = nullptr);

private:
    void setupUI(void);
};

#endif // DOWNLOADRESULTDIALOG_H

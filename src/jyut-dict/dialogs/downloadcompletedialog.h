#ifndef DOWNLOADCOMPLETEDIALOG_H
#define DOWNLOADCOMPLETEDIALOG_H

#include "dialogs/defaultdialog.h"

#include <QEvent>
#include <QString>

class DownloadCompleteDialog : public DefaultDialog
{
    Q_OBJECT
public:
    explicit DownloadCompleteDialog(const QString &reason,
                                    const QString &description,
                                    QWidget *parent = nullptr);

private:
    void setupUI(void);
};

#endif // DOWNLOADCOMPLETEDIALOG_H

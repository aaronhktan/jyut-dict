#ifndef TRANSCRIPTIONERRORDIALOG_H
#define TRANSCRIPTIONERRORDIALOG_H

#include "dialogs/defaultdialog.h"

#include <QEvent>
#include <QString>

class TranscriptionErrorDialog : public DefaultDialog
{
    Q_OBJECT
public:
    explicit TranscriptionErrorDialog(const QString &reason,
                                      const QString &description,
                                      QWidget *parent = nullptr);

private:
    void setupUI(void);
};

#endif // TRANSCRIPTIONERRORDIALOG_H

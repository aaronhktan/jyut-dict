#ifndef DICTIONARYTABFAILUREDIALOG_H
#define DICTIONARYTABFAILUREDIALOG_H

#include "dialogs/defaultdialog.h"

#include <QEvent>
#include <QString>

class DictionaryTabFailureDialog : public DefaultDialog
{
    Q_OBJECT
public:
    explicit DictionaryTabFailureDialog(const QString &reason,
                                        const QString &description,
                                        QWidget *parent = nullptr);

private:
    void setupUI(void);
};

#endif // DICTIONARYTABFAILUREDIALOG_H

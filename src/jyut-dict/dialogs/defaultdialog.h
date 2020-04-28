#ifndef ERRORDIALOG_H
#define ERRORDIALOG_H

#include <QMessageBox>
#include <QString>

class DefaultDialog : public QMessageBox
{
public:
    explicit DefaultDialog(const QString &reason,
                           const QString &description,
                           QWidget *parent = nullptr);
    virtual ~DefaultDialog();

    void setWidth(int width);
};

#endif // ERRORDIALOG_H

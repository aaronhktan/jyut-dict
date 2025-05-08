#ifndef ERRORDIALOG_H
#define ERRORDIALOG_H

#include <QEvent>
#include <QMessageBox>
#include <QSettings>
#include <QString>

class DefaultDialog : public QMessageBox
{
public:
    explicit DefaultDialog(const QString &reason,
                           const QString &description,
                           QWidget *parent = nullptr);
    virtual ~DefaultDialog() override = default;

    void changeEvent(QEvent *event) override;

    void setWidth(int width);

protected:
    void translateUI(void);
    void setStyle(bool use_dark);
    void deselectButtons(void);

private:
    void setupUI(const QString &reason, const QString &description);

    std::unique_ptr<QSettings> _settings;
};

#endif // ERRORDIALOG_H

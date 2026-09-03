#ifndef DEFAULTDIALOG_H
#define DEFAULTDIALOG_H

#include <QMessageBox>
#include <QSettings>

class QEvent;
class QString;

// The DefaultDialog class is the base class for many dialogs
// in the application. It provides basic shared functionatlity,
// such as setting the style of buttons and displaying text.

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

#endif // DEFAULTDIALOG_H

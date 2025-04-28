#ifndef WELCOMEWINDOW_H
#define WELCOMEWINDOW_H

#include <QEvent>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QTextEdit>
#include <QWidget>

// The Welcome Window displays setup options for a new user.

class WelcomeWindow : public QWidget
{
    Q_OBJECT

public:
    explicit WelcomeWindow(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

private:
    void setupUI(void);
    void translateUI(void);
    void setStyle(bool use_dark);

    QLabel *_iconLabel;
    QLabel *_titleLabel;
    QLabel *_messageLabel;

    QTextEdit *_descriptionTextEdit;
    QWidget *_spacer;

    QPushButton *_noButton;
    QPushButton *_okButton;

    QGridLayout *_dialogLayout;

    std::unique_ptr<QSettings> _settings;

signals:
    void welcomeCompleted(void);

public slots:
    void noAction(void);
    void OKAction(void);
};

#endif // WELCOMEWINDOW_H

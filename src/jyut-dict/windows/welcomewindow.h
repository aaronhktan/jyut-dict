#ifndef WELCOMEWINDOW_H
#define WELCOMEWINDOW_H

#include <QEvent>
#include <QGridLayout>
#include <QKeyEvent>
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
    void keyPressEvent(QKeyEvent *event) override;

private:
    void setupUI(void);
    void translateUI(void);
    void setStyle(bool use_dark);

#ifdef Q_OS_WIN
    // On Windows, the window widget's background colour
    // can't be changed. The workaround is to create
    // a new QWidget (whose background colour can be changed)
    // and then add it to the window's layout.
    QWidget *_innerWidget;
    QGridLayout *_outerWidgetLayout;
#endif

    QLabel *_iconLabel;
    QWidget *_leftSpacer;
    QWidget *_rightSpacer;

    QLabel *_titleLabel;
    QLabel *_messageLabel;

    QWidget *_buttonSpacer;

    QPushButton *_noButton;
    QPushButton *_okButton;

    QWidget *_bottomSpacer;

    QGridLayout *_dialogLayout;

    std::unique_ptr<QSettings> _settings;

signals:
    void welcomeCompleted(void);

public slots:
    void noAction(void);
    void OKAction(void);
};

#endif // WELCOMEWINDOW_H

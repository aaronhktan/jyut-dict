#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include <QEvent>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

// The about window displays information about the program.

class AboutWindow : public QWidget
{
    Q_OBJECT

public:
    AboutWindow(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    void setupUI();
    void translateUI();

    void setStyle(bool use_dark);

    bool _paletteRecentlyChanged = false;

    QLabel *_iconLabel;
    QLabel *_titleLabel;
    QLabel *_versionLabel;
    QLabel *_descriptionLabel;
    QLabel *_messageLabel;

    QPushButton *_websiteButton;
    QPushButton *_githubButton;

    QGridLayout *_windowLayout;
};

#endif // ABOUTWINDOW_H

#ifndef UPDATEWINDOW_H
#define UPDATEWINDOW_H

#include "logic/utils/utils.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>

#include <string>

class UpdateWindow : public QWidget
{
    Q_OBJECT

public:
    explicit UpdateWindow(QWidget *parent = nullptr,
                          std::string versionNumber=Utils::CURRENT_VERSION,
                          std::string url=Utils::GITHUB_LINK,
                          std::string description="");
    ~UpdateWindow();

private:
    QLabel *_iconLabel;
    QLabel *_titleLabel;
    QLabel *_messageLabel;

    QTextEdit *_descriptionTextEdit;
    QWidget *_spacer;

    QPushButton *_noButton;
    QPushButton *_showMoreButton;
    QPushButton *_okButton;

    QGridLayout *_dialogLayout;

    std::string _url;

signals:

public slots:
    void showDetails();
    void hideDetails();
    void noAction();
    void OKAction();
};

#endif // UPDATEWINDOW_H

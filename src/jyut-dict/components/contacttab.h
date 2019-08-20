#ifndef CONTACTTAB_H
#define CONTACTTAB_H

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

class ContactTab : public QWidget
{
public:
    explicit ContactTab(QWidget *parent = nullptr);

private:
    void setupUI();

    QLabel *_iconLabel;
    QLabel *_titleLabel;
    QLabel *_messageLabel;

    QPushButton *_emailButton;
    QPushButton *_donateButton;
    QPushButton *_githubButton;

    QGridLayout *_tabLayout;
};

#endif // CONTACTTAB_H

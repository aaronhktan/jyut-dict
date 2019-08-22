#ifndef CONTACTTAB_H
#define CONTACTTAB_H

#include <QEvent>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

// THe contact tab displays contact and donations information.

class ContactTab : public QWidget
{
public:
    explicit ContactTab(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

private:
    void setupUI();

    void setStyle(bool use_dark);

    bool _paletteRecentlyChanged = false;

    QGroupBox *_box;

    QLabel *_iconLabel;
    QLabel *_titleLabel;
    QLabel *_messageLabel;

    QPushButton *_emailButton;
    QPushButton *_donateButton;
    QPushButton *_githubButton;

    QFrame *_divider;

    QLabel *_otherSourcesLabel;
    QLabel *_otherSources;

    QGridLayout *_boxLayout;
    QGridLayout *_tabLayout;
};

#endif // CONTACTTAB_H

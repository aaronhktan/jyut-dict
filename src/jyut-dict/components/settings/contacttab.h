#ifndef CONTACTTAB_H
#define CONTACTTAB_H

#include <QWidget>

// The contact tab displays contact and donations information.

class QEvent;
class QGridLayout;
class QGroupBox;
class QLabel;
class QPushButton;

class ContactTab : public QWidget
{
    Q_OBJECT

public:
    explicit ContactTab(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    void setupUI();
    void translateUI();

    void setStyle(bool use_dark);

    bool _paletteRecentlyChanged = false;

    QGroupBox *_box;

    QLabel *_iconLabel;
    QLabel *_titleLabel;
    QLabel *_messageLabel;

    QPushButton *_emailButton;
    QPushButton *_donateButton;
    QPushButton *_githubButton;

    QLabel *_otherSourcesLabel;
    QLabel *_otherSources;

    QGridLayout *_boxLayout;
    QGridLayout *_tabLayout;
};

#endif // CONTACTTAB_H

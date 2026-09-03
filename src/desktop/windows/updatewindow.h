#ifndef UPDATEWINDOW_H
#define UPDATEWINDOW_H

#include "logic/utils/utils.h"

#include <QWidget>

#include <string>

class QEvent;
class QGridLayout;
class QLabel;
class QPushButton;
class QSettings;
class QTextEdit;

// The Update Available Window displays an update notification to the user.

class UpdateAvailableWindow : public QWidget
{
    Q_OBJECT

public:
    explicit UpdateAvailableWindow(QWidget *parent = nullptr,
                                   std::string versionNumber
                                   = Utils::CURRENT_VERSION,
                                   std::string url = Utils::GITHUB_LINK,
                                   std::string description = "");

    void changeEvent(QEvent *event) override;

private:
    void setupUI();
    void translateUI();
    void setStyle(bool use_dark);

    std::string _versionNumber;
    std::string _url;
    std::string _description;

    QLabel *_iconLabel;
    QLabel *_titleLabel;
    QLabel *_messageLabel;

    QTextEdit *_descriptionTextEdit;
    QWidget *_spacer;

    QPushButton *_noButton;
    QPushButton *_showMoreButton;
    QPushButton *_okButton;

    QGridLayout *_dialogLayout;

    std::unique_ptr<QSettings> _settings;

public slots:
    void showDetails();
    void hideDetails();
    void noAction();
    void OKAction();
};

#endif // UPDATEWINDOW_H

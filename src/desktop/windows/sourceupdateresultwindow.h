#ifndef SOURCEUPDATERESULTWINDOW_H
#define SOURCEUPDATERESULTWINDOW_H

#include <QWidget>

#include <string>

class QEvent;
class QGridLayout;
class QLabel;
class QPushButton;
class QSettings;
class QTextEdit;

// The Source Update Result Window displays the status of sources after an attempted update.

class SourceUpdateResultWindow : public QWidget
{
    Q_OBJECT
public:
    enum class UpdateResult {
        kAllSucceeded,
        kSomeSucceeded,
        kNoneSucceeded,
    };

    explicit SourceUpdateResultWindow(UpdateResult result,
                                      std::string description,
                                      QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

private:
    void setupUI();
    void translateUI();
    void setStyle(bool use_dark);

    UpdateResult _result;
    std::string _description;

    QLabel *_iconLabel;
    QLabel *_titleLabel;
    QLabel *_messageLabel;

    QTextEdit *_descriptionTextEdit;
    QWidget *_spacer;

    QPushButton *_showMoreButton;
    QPushButton *_okButton;

    QGridLayout *_dialogLayout;

    std::unique_ptr<QSettings> _settings;

public slots:
    void showDetails();
    void hideDetails();
    void OKAction();
};

#endif // SOURCEUPDATERESULTWINDOW_H

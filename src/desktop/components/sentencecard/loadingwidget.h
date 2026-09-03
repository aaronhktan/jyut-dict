#ifndef LOADINGWIDGET_H
#define LOADINGWIDGET_H

#include <QWidget>

class QEvent;
class QLabel;
class QMovie;
class QVBoxLayout;

// The LoadingWidget displays a sentence and a circular spinner.

class LoadingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LoadingWidget(QWidget *parent = nullptr);
    ~LoadingWidget() override;

    void changeEvent(QEvent *event) override;

private:
    void setupUI(void);
    void translateUI(void);
    void setStyle(bool use_dark);

    bool _paletteRecentlyChanged = false;

    QVBoxLayout *_layout;

    QLabel *_movieLabel;
    QLabel *_descriptiveLabel;

    QMovie *_movie;
};

#endif // LOADINGWIDGET_H

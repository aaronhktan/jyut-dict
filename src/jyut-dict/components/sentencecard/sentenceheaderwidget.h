#ifndef SENTENCEHEADERWIDGET_H
#define SENTENCEHEADERWIDGET_H

#include <QEvent>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

class SentenceHeaderWidget : public QWidget
{
public:
    explicit SentenceHeaderWidget(QWidget *parent = nullptr);
    explicit SentenceHeaderWidget(std::string title,
                                    QWidget *parent = nullptr);

    void changeEvent(QEvent *event);

    void setCardTitle(std::string title);

private:
    void setStyle(bool use_dark);

    bool _paletteRecentlyChanged = false;

    QLabel *_titleLabel;
    QVBoxLayout *_layout;

};

#endif // SENTENCEHEADERWIDGET_H

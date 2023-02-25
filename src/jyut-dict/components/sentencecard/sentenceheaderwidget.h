#ifndef SENTENCEHEADERWIDGET_H
#define SENTENCEHEADERWIDGET_H

#include <QEvent>
#include <QLabel>
#include <QSettings>
#include <QVBoxLayout>
#include <QWidget>

// The SentenceHeaderWidget provides a header for the sentence card

class SentenceHeaderWidget : public QWidget
{
public:
    explicit SentenceHeaderWidget(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

    void setCardTitle(const std::string &title);

private:
    void setStyle(bool use_dark);

    bool _paletteRecentlyChanged = false;

    std::unique_ptr<QSettings> _settings;

    QLabel *_titleLabel;
    QVBoxLayout *_layout;

};

#endif // SENTENCEHEADERWIDGET_H

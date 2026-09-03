#ifndef SENTENCEHEADERWIDGET_H
#define SENTENCEHEADERWIDGET_H

#include <QSettings>
#include <QWidget>

class QEvent;
class QLabel;
class QVBoxLayout;

// The SentenceHeaderWidget provides a header for the sentence card

class SentenceHeaderWidget : public QWidget
{
public:
    explicit SentenceHeaderWidget(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

    void setSource(const std::string &title);

private:
    void translateUI();
    void setStyle(bool use_dark);

    bool _paletteRecentlyChanged = false;

    std::string _source;

    std::unique_ptr<QSettings> _settings;

    QLabel *_titleLabel;
    QVBoxLayout *_layout;

};

#endif // SENTENCEHEADERWIDGET_H

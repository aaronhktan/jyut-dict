#ifndef EXAMPLEHEADERWIDGET_H
#define EXAMPLEHEADERWIDGET_H

#include <QSettings>
#include <QWidget>

class QEvent;
class QLabel;
class QVBoxLayout;

// The ExampleHeaderWidget provides a header for the example card

class ExampleHeaderWidget : public QWidget
{
public:
    explicit ExampleHeaderWidget(QWidget *parent = nullptr);

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

#endif // EXAMPLEHEADERWIDGET_H

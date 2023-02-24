#ifndef RELATEDBUTTONHEADERWIDGET_H
#define RELATEDBUTTONHEADERWIDGET_H

#include <QEvent>
#include <QLabel>
#include <QSettings>
#include <QVBoxLayout>
#include <QWidget>

// The RelatedButtonHeaderWidget is a header for the related button

class RelatedButtonHeaderWidget : public QWidget
{
public:
    explicit RelatedButtonHeaderWidget(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

private:
    void translateUI();

    void setStyle(bool use_dark);

    bool _paletteRecentlyChanged = false;

    std::unique_ptr<QSettings> _settings;

    QLabel *_titleLabel;
    QVBoxLayout *_layout;
};

#endif // RELATEDBUTTONHEADERWIDGET_H

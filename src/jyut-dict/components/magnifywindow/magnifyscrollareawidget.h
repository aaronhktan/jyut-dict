#ifndef MAGNIFYSCROLLAREAWIDGET_H
#define MAGNIFYSCROLLAREAWIDGET_H

#include "logic/entry/entry.h"

#include <QLabel>
#include <QSettings>
#include <QVBoxLayout>
#include <QWidget>

// MagnifyScrollAreaWidget contains the widgets that are displayed in the
// MagnifyScrollArea. This is the window that shows an enlarged version of the
// simplified and traditional forms of the entry.
class MagnifyScrollAreaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MagnifyScrollAreaWidget(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

    void setEntry(const Entry &entry);

private:
    void translateUI(void);
    void setStyle(bool use_dark);

    bool _paletteRecentlyChanged = false;
    std::unique_ptr<QSettings> _settings;

    QVBoxLayout *_scrollAreaLayout;

    QWidget *_widget;
    QVBoxLayout *_widgetLayout;
    QLabel *_traditionalLabelLabel;
    QLabel *_traditionalLabel;
    QLabel *_simplifiedLabelLabel;
    QLabel *_simplifiedLabel;
};

#endif // MAGNIFYSCROLLAREAWIDGET_H

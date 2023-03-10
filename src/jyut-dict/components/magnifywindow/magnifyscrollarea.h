#ifndef MAGNIFYSCROLLAREA_H
#define MAGNIFYSCROLLAREA_H

#include "components/magnifywindow/magnifyscrollareawidget.h"
#include "logic/entry/entry.h"

#include <QResizeEvent>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

// MagnifyScrollArea enables scrolling of the MagnifyScrollAreaWidget.
class MagnifyScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit MagnifyScrollArea(QWidget *parent = nullptr);

    void keyPressEvent(QKeyEvent *event) override;

    void setEntry(const Entry &entry);

private:
    void resizeEvent(QResizeEvent *event) override;

    std::unique_ptr<QSettings> _settings;
    Entry _entry;
    bool _entryIsValid = false;

    MagnifyScrollAreaWidget *_scrollAreaWidget;

public slots:
    void updateStyleRequested(void);
};

#endif // MAGNIFYSCROLLAREA_H

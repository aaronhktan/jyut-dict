#ifndef HANDWRITINGWINDOW_H
#define HANDWRITINGWINDOW_H

#include "components/handwriting/handwritingpanel.h"
#include "logic/handwriting/handwritingwrapper.h"

#include <QEvent>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QTextEdit>
#include <QWidget>

// The Handwriting Window displays a panel to write a character,
// and a list of possible results.

class HandwritingWindow : public QWidget
{
    Q_OBJECT

public:
    explicit HandwritingWindow(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void setupUI(void);
    void translateUI(void);
    void setStyle(bool use_dark);

#ifdef Q_OS_WIN
    // On Windows, the window widget's background colour
    // can't be changed. The workaround is to create
    // a new QWidget (whose background colour can be changed)
    // and then add it to the window's layout.
    QWidget *_innerWidget;
    QGridLayout *_outerWidgetLayout;
#endif

    HandwritingPanel *_panel;

    std::vector<QPushButton *> _buttons;
    QPushButton *_clearButton;
    QPushButton *_backspaceButton;
    QPushButton *_doneButton;

    QGridLayout *_layout;

    std::unique_ptr<QSettings> _settings;
    std::unique_ptr<HandwritingWrapper> _handwritingWrapper;

signals:
    void scriptSelected(Handwriting::Script script);
    void characterChosen(QString character);

public slots:
};

#endif // HANDWRITINGWINDOW_H

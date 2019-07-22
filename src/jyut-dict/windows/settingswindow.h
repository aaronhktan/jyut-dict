#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QLayout>
#include <QMainWindow>
#include <QStackedWidget>
#include <QToolBar>

class SettingsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

private:
    QWidget *_parent;

    QLayout *_layout;

    QStackedWidget *_contentStackedWidget;
    QToolBar *_toolBar;

    void openFirstTab();
    void openSecondTab();
};

#endif // SETTINGSWINDOW_H

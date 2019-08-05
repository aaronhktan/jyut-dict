#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include "logic/database/sqldatabasemanager.h"

#include <QLayout>
#include <QMainWindow>
#include <QStackedWidget>
#include <QToolBar>

#include <memory>

constexpr auto NUM_OF_TABS = 4;

class SettingsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SettingsWindow(std::shared_ptr<SQLDatabaseManager> manager,
                            QWidget *parent = nullptr);
    ~SettingsWindow();

private:
    QWidget *_parent;

    QLayout *_layout;

    QStackedWidget *_contentStackedWidget;
    QToolBar *_toolBar;

    std::shared_ptr<SQLDatabaseManager> _manager;

    void openTab(int tabIndex);
};

#endif // SETTINGSWINDOW_H

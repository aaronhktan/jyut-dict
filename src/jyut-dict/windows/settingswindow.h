#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include "logic/database/sqldatabasemanager.h"

#include <QAction>
#include <QEvent>
#include <QLayout>
#include <QMainWindow>
#include <QStackedWidget>
#include <QToolBar>
#include <QToolButton>

#include <memory>
#include <vector>

// The SettingsWindow allows users to modify settings. Surprise!

constexpr auto NUM_OF_TABS = 4;

class SettingsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SettingsWindow(std::shared_ptr<SQLDatabaseManager> manager,
                            QWidget *parent = nullptr);
    ~SettingsWindow();

    void changeEvent(QEvent *event);

private:
    QWidget *_parent;

    QLayout *_layout;

    QStackedWidget *_contentStackedWidget;
    QToolBar *_toolBar;

    std::vector<QToolButton *> _toolButtons;
    std::vector<QAction *> _actions;

    std::shared_ptr<SQLDatabaseManager> _manager;

    bool _paletteRecentlyChanged;

    void setStyle(bool use_dark);
    void setButtonIcon(bool use_dark, int index);
    void openTab(int tabIndex);
};

#endif // SETTINGSWINDOW_H

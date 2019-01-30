#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "components/mainsplitter.h"
#include "components/maintoolbar.h"

#include <QMainWindow>

// As its name suggests, is the main window of the application
// Contains a toolbar (for searching), and splitter (for results/detail)

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    MainToolBar *_mainToolBar;
    MainSplitter *_mainSplitter;
};

#endif // MAINWINDOW_H

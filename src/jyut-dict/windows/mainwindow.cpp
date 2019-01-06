#include "windows/mainwindow.h"

#include <logic/entry/sentence.h>

#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle("Jyut Dictionary");
    setMinimumSize(QSize(800, 600));

    _mainToolBar = new MainToolBar(this);
    addToolBar(_mainToolBar);
    setUnifiedTitleAndToolBarOnMac(true);

    _mainSplitter = new MainSplitter(this);
    setCentralWidget(_mainSplitter);
}

MainWindow::~MainWindow()
{
    delete _mainToolBar;
    delete _mainSplitter;
}

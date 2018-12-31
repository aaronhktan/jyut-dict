#include "windows/mainwindow.h"

#include <logic/entry/sentence.h>

#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    this->setWindowTitle("Jyut Dictionary");
    this->setMinimumSize(QSize(800, 600));

    mainToolBar = new MainToolBar;
    this->addToolBar(mainToolBar);
    this->setUnifiedTitleAndToolBarOnMac(true);

    mainSplitter = new MainSplitter;
    this->setCentralWidget(mainSplitter);
}

MainWindow::~MainWindow()
{
}

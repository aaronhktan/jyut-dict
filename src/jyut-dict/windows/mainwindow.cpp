#include "windows/mainwindow.h"

#include <stdlib.h>

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

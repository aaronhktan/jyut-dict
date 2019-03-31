#include "windows/mainwindow.h"

#include <logic/entry/sentence.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle("Jyut Dictionary");
#ifdef Q_OS_LINUX
    setMinimumSize(QSize(600, 600));
#else
    setMinimumSize(QSize(800, 600));
#endif

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

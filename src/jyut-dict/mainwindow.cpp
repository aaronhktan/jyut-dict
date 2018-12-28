#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <stdlib.h>

#include <QToolBar>
#include <QHBoxLayout>
#include <QIcon>
#include <QLineEdit>
#include <QListView>
#include <QScrollArea>
#include <QSplitter>
#include <QToolButton>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    QLineEdit *lineEdit = new QLineEdit;
    lineEdit->setPlaceholderText("Search");

    QIcon icon = QIcon("/Users/aaron/Downloads/glass.png");
    QIcon clear = QIcon("/Users/aaron/Downloads/x.png");

    lineEdit->addAction(icon, QLineEdit::LeadingPosition);
    lineEdit->addAction(clear, QLineEdit::TrailingPosition);
    lineEdit->setStyleSheet("QLineEdit { \
                            border-radius: 3px; \
                            font-size: 12px; \
                            padding-top: 4px; \
                            padding-bottom: 4px}");

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(lineEdit);

    QWidget *toolbarWidget = new QWidget;
    toolbarWidget->setLayout(mainLayout);

    QToolBar *toolbar = this->addToolBar("main");
    toolbar->addWidget(toolbarWidget);
    toolbar->setMovable(false);
    toolbar->setFloatable(false);

    QSplitter *splitter = new QSplitter;
    QScrollArea *andy = new QScrollArea;
    QListView *listView = new QListView;

    splitter->addWidget(listView);
    splitter->addWidget(andy);
    splitter->setHandleWidth(0);
    splitter->setCollapsible(0, false);
    splitter->setCollapsible(1, false);
    splitter->setStyleSheet("QSplitter::handle \
                            { \
                                background-color: rgb(0, 0, 0); \
                            }");

    this->setUnifiedTitleAndToolBarOnMac(true);
    this->setWindowTitle("Jyut Dictionary");
    this->setCentralWidget(splitter);
    this->setMinimumSize(QSize(400, 300));
}

MainWindow::~MainWindow()
{
}

#include "settingswindow.h"

#include "components/dictionarytab.h"

#include <QAction>
#include <QActionGroup>
#include <QVBoxLayout>
#include <QToolButton>

SettingsWindow::SettingsWindow(QWidget *parent)
    : QMainWindow{parent, Qt::Window},
      _parent{parent}
{
//    _layout = new QVBoxLayout{this};

    _contentStackedWidget = new QStackedWidget{this};
    _toolBar = new QToolBar{this};
    addToolBar(_toolBar);
    setUnifiedTitleAndToolBarOnMac(true);
    _toolBar->setMovable(false);
    _toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    DictionaryTab *_dictionaryTab = new DictionaryTab(this, "hello");
    DictionaryTab *_seconddictionaryTab = new DictionaryTab(this, "whoa");

    _contentStackedWidget->addWidget(_dictionaryTab);
    _contentStackedWidget->addWidget(_seconddictionaryTab);
    QToolButton *_toolButton1 = new QToolButton();
    QToolButton *_toolButton2 = new QToolButton();

    QAction *action1 = new QAction{tr("Dictionary1"), this};
    action1->setIcon(QIcon(":/images/search_inverted.png"));
    action1->setCheckable(true);
    connect(action1, &QAction::triggered, this, &SettingsWindow::openFirstTab);
    _toolButton1->setDefaultAction(action1);
    _toolButton1->setStyleSheet("QToolButton { "
                                "   border-top-left-radius: 4px;"
                                "   border-top-right-radius: 4px;"
                                "   margin: 0px; "
                                "}"
                                " "
                                "QToolButton:checked { "
                                "   border-top-left-radius: 4px;"
                                "   border-top-right-radius: 4px;"
                                "   margin: 0px; "
                                "   background-color: #767676; "
                                "}");
    _toolButton1->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    QAction *action2 = new QAction{tr("boop"), this};
    action2->setIcon(QIcon(":/images/x_inverted.png"));
    action2->setCheckable(true);
    connect(action2, &QAction::triggered, this, &SettingsWindow::openSecondTab);
    _toolButton2->setDefaultAction(action2);
    _toolButton2->setStyleSheet("QToolButton { "
                                "   border-top-left-radius: 4px;"
                                "   border-top-right-radius: 4px;"
                                "   margin: 0px; "
                                "}"
                                " "
                                "QToolButton:checked { "
                                "   border-top-left-radius: 4px;"
                                "   border-top-right-radius: 4px;"
                                "   margin: 0px; "
                                "   background-color: #767676; "
                                "}");
    _toolButton2->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    QActionGroup *_navigationActionGroup = new QActionGroup{this};
    _navigationActionGroup->setExclusive(true);
    _navigationActionGroup->addAction(action1);
    _navigationActionGroup->addAction(action2);
    //    _navigationTabWidget->setTabIcon(0, QIcon(":/images/x_inverted.png"));
    //    _toolBar->addAction(QIcon(":/images/x_inverted.png"), "Dictionaries", this, &SettingsWindow::openFirstTab);
    //    _toolBar->addAction(QIcon(":/images/x_inverted.png"), "SecondDictionaries", this, &SettingsWindow::openSecondTab);
    //    _layout->addWidget(_toolBar);


    _toolBar->addWidget(_toolButton1);
    _toolBar->addWidget(_toolButton2);

    _toolButton1->click();

    //    _layout->addWidget(_navigationTabWidget);
    setCentralWidget(_contentStackedWidget);

    setWindowTitle("Preferences");

    setMinimumSize(600, 400);
    resize(sizeHint());

    setAttribute(Qt::WA_DeleteOnClose);
}

SettingsWindow::~SettingsWindow()
{

}

void SettingsWindow::openFirstTab()
{
    _contentStackedWidget->setCurrentIndex(0);
}

void SettingsWindow::openSecondTab()
{
    _contentStackedWidget->setCurrentIndex(1);
}

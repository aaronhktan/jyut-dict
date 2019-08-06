#include "settingswindow.h"

#include "components/dictionarytab.h"
#include "logic/utils/utils_mac.h"

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QGuiApplication>
#include <QPalette>
#include <QVBoxLayout>
#include <QToolButton>

SettingsWindow::SettingsWindow(std::shared_ptr<SQLDatabaseManager> manager,
                               QWidget *parent)
    : QMainWindow{parent, Qt::Window},
      _parent{parent}
{
    _manager = manager;

    _contentStackedWidget = new QStackedWidget{this};
    _toolBar = new QToolBar{this};
#ifdef Q_OS_WIN
    _toolBar->setStyleSheet("QToolBar {"
                            "   background-color: white;"
                            "}");
#endif
    addToolBar(_toolBar);
    setUnifiedTitleAndToolBarOnMac(true);
    _toolBar->setMovable(false);
    _toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    int r, g, b, a;
#ifdef Q_OS_MAC
    Utils::getAppleControlAccentColor().getRgb(&r, &g, &b, &a);
    a = 0.8;
#else
    QColor(204, 0, 1).getRgb(&r, &g, &b, &a);
#endif

    std::vector<QToolButton *> _toolButtons;
    std::vector<QAction *> _actions;
    QActionGroup *_navigationActionGroup = new QActionGroup{this};
    _navigationActionGroup->setExclusive(true);

    for (int i = 0; i < NUM_OF_TABS; i++) {
        _actions.push_back(new QAction{this});
        _actions.back()->setCheckable(true);
        connect(_actions.back(), &QAction::triggered, this, [=] { openTab(i); });
        _navigationActionGroup->addAction(_actions.back());

        _toolButtons.push_back(new QToolButton{this});
#ifdef Q_OS_MAC
        QString style{"QToolButton { "
                      "   border-top-left-radius: 4px;"
                      "   border-top-right-radius: 4px;"
                      "   margin: 0px; "
                      "}"
                      " "
                      "QToolButton:checked { "
                      "   border-top-left-radius: 4px;"
                      "   border-top-right-radius: 4px;"
                      "   margin: 0px; "
                      "   background-color: rgba(%1, %2, %3, %4); "
                      "}"};
#else
        QString style{"QToolButton { "
                      "   border-radius: 2px;"
                      "   margin: 0px; "
                      "}"
                      " "
                      "QToolButton:checked { "
                      "   border-radius: 2px;"
                      "   margin: 0px; "
                      "   background-color: rgba(%1, %2, %3, %4); "
                      "   color: #FFFFFF;"
                      "}"};
#endif
        _toolButtons.back()->setStyleSheet(
            style.arg(std::to_string(r).c_str(),
                      std::to_string(g).c_str(),
                      std::to_string(b).c_str(),
                      std::to_string(a).c_str()));
        _toolButtons.back()->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        _toolButtons.back()->setDefaultAction(_actions.back());
        _toolBar->addWidget(_toolButtons.back());
    }

    // For these images, export as 96px width, and center on 120px canvas.
    _actions[0]->setText(tr("General"));
    _actions[0]->setIcon(QIcon(":/images/settings_inverted.png"));
    QWidget *generalTab = new QWidget{this};
    _contentStackedWidget->addWidget(generalTab);

    _actions[1]->setText(tr("Dictionaries"));
    _actions[1]->setIcon(QIcon(":/images/book_inverted.png"));
    DictionaryTab *dictionaryTab = new DictionaryTab{_manager, this};
    _contentStackedWidget->addWidget(dictionaryTab);

    _actions[2]->setText(tr("Advanced"));
    _actions[2]->setIcon(QIcon(":/images/sliders_inverted.png"));
    QWidget *advancedTab = new QWidget{this};
    _contentStackedWidget->addWidget(advancedTab);

    _actions[3]->setText(tr("Contact"));
    _actions[3]->setIcon(QIcon(":/images/help_inverted.png"));
    QWidget *contactTab = new QWidget{this};
    _contentStackedWidget->addWidget(contactTab);

    setCentralWidget(_contentStackedWidget);

    _toolButtons[0]->click();

    setWindowTitle(tr("Preferences"));
    setMinimumSize(600, 400);
    resize(sizeHint());
    layout()->setSizeConstraint(QLayout::SetFixedSize);
    move(parent->x() + (parent->width() - size().width()) / 2,
         parent->y() + (parent->height() - size().height()) / 2);

    setAttribute(Qt::WA_DeleteOnClose);
}

SettingsWindow::~SettingsWindow()
{

}

void SettingsWindow::openTab(int tabIndex)
{
    for (int index = 0; index < _contentStackedWidget->count(); index++) {
        // Ignore sizehint of non-active widgets
        QSizePolicy::Policy policy = QSizePolicy::Ignored;
        if (index == tabIndex) {
            policy = QSizePolicy::Expanding;
        }

        QWidget *widget = _contentStackedWidget->widget(index);
        widget->setSizePolicy(policy, policy);
    }

    _contentStackedWidget->setCurrentIndex(tabIndex);
}

#include "settingswindow.h"

#include "components/dictionarytab.h"
#include "components/settingstab.h"
#include "logic/utils/utils_mac.h"

#include <QActionGroup>
#include <QGuiApplication>
#include <QPalette>
#include <QTimer>

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
#elif defined(Q_OS_LINUX)
    QColor color = QGuiApplication::palette().color(QPalette::AlternateBase);
    _toolBar->setStyleSheet(QString("QToolBar {"
                            "   background: transparent;"
                            "   background-color: rgba(%1, %2, %3, %4);"
                            "   border-bottom: 1px solid lightgray;"
                            "   padding-bottom: 3px;"
                            "   padding-top: 3px;"
                            "}")
                            .arg(std::to_string(color.red()).c_str())
                            .arg(std::to_string(color.green()).c_str())
                            .arg(std::to_string(color.blue()).c_str())
                            .arg(0.7));
#endif
    addToolBar(_toolBar);
    setUnifiedTitleAndToolBarOnMac(true);
    _toolBar->setMovable(false);

    QActionGroup *_navigationActionGroup = new QActionGroup{this};
    _navigationActionGroup->setExclusive(true);

    for (int i = 0; i < NUM_OF_TABS; i++) {
        _actions.push_back(new QAction{this});
        _actions.back()->setCheckable(true);
        connect(_actions.back(), &QAction::triggered, this, [=] { openTab(i); });
        _navigationActionGroup->addAction(_actions.back());

        _toolButtons.push_back(new QToolButton{this});
        _toolButtons.back()->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        _toolButtons.back()->setDefaultAction(_actions.back());
        _toolBar->addWidget(_toolButtons.back());
    }

    // Customize the look of the toolbar to fit in better with platform styles
#ifdef Q_OS_MAC
    if (!system("defaults read -g AppleInterfaceStyle")) {
        setStyle(/* use_dark = */true);
    } else {
        setStyle(/* use_dark = */false);
    }
#else
    setStyle(/* use_dark = */false);
#endif

    _actions[0]->setText(tr("General"));
    SettingsTab *generalTab = new SettingsTab{this};
    _contentStackedWidget->addWidget(generalTab);

    _actions[1]->setText(tr("Dictionaries"));
    DictionaryTab *dictionaryTab = new DictionaryTab{_manager, this};
    _contentStackedWidget->addWidget(dictionaryTab);

    _actions[2]->setText(tr("Advanced"));
    QLabel *advancedTab = new QLabel{"This page is intentionally left blank.",
                                     this};
    advancedTab->setMinimumSize(400, 250);
    advancedTab->setAlignment(Qt::AlignCenter);
    _contentStackedWidget->addWidget(advancedTab);

    _actions[3]->setText(tr("Contact"));
    QLabel *contactTab = new QLabel{"This page is intentionally left blank.",
                                    this};
    contactTab->setMinimumSize(400, 250);
    contactTab->setAlignment(Qt::AlignCenter);
    _contentStackedWidget->addWidget(contactTab);

    setCentralWidget(_contentStackedWidget);

    _toolButtons[0]->click();

#ifdef Q_OS_MAC
    setWindowTitle(tr("Preferences"));
#else
    setWindowTitle(tr("Settings"));
#endif
    setMinimumSize(600, 400);
    resize(sizeHint());
    layout()->setSizeConstraint(QLayout::SetFixedSize);
    move(parent->x() + (parent->width() - sizeHint().width()) / 2,
         parent->y() + (parent->height() - sizeHint().height()) / 2);

    setAttribute(Qt::WA_DeleteOnClose);
}

SettingsWindow::~SettingsWindow()
{

}

void SettingsWindow::changeEvent(QEvent *event)
{
#if defined(Q_OS_DARWIN)
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(100, [=]() { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        if (!system("defaults read -g AppleInterfaceStyle")) {
            setStyle(/* use_dark = */ true);
        } else {
            setStyle(/* use_dark = */ false);
        }
    }
#endif
    QMainWindow::changeEvent(event);
}

void SettingsWindow::setStyle(bool use_dark)
{
    // Set background color of tabs in toolbar
    int r, g, b, a;
#ifdef Q_OS_MAC
    Utils::getAppleControlAccentColor().getRgb(&r, &g, &b, &a);
#else
    QColor(204, 0, 1).getRgb(&r, &g, &b, &a);
#endif

#ifdef Q_OS_MAC
    QString style;
    if (use_dark) {
        style = "QToolButton { "
                "   border-top-left-radius: 4px; "
                "   border-top-right-radius: 4px; "
                "   margin: 0px; "
                "}"
                " "
                "QToolButton:checked { "
                "   border-top-left-radius: 4px; "
                "   border-top-right-radius: 4px; "
                "   margin: 0px; "
                "   background-color: rgba(%1, %2, %3, %4); "
                "}";
    } else {
        style = "QToolButton { "
                "   border-top-left-radius: 4px; "
                "   border-top-right-radius: 4px; "
                "   margin: 0px; "
                "}"
                " "
                "QToolButton:checked { "
                "   border-top-left-radius: 4px; "
                "   border-top-right-radius: 4px; "
                "   margin: 0px; "
                "   background-color: rgba(%1, %2, %3, %4); "
                "   color: #FFFFFF; "
                "}";
    }
#else
    QString style{"QToolButton { "
                  "   border-radius: 2px; "
                  "   margin: 0px; "
                  "}"
                  " "
                  "QToolButton:checked { "
                  "   border-radius: 2px; "
                  "   margin: 0px; "
                  "   background-color: rgba(%1, %2, %3, %4); "
                  "   color: #FFFFFF; "
                  "}"};
#endif
    for (auto button : _toolButtons) {
        button->setStyleSheet(style.arg(std::to_string(r).c_str(),
                                        std::to_string(g).c_str(),
                                        std::to_string(b).c_str(),
                                        std::to_string(a).c_str()));
    }

    setButtonIcon(use_dark, _contentStackedWidget->currentIndex());
}

void SettingsWindow::setButtonIcon(bool use_dark, int index)
{
    // For these images, export as 96px width, and center on 120px canvas.
    QIcon settings = QIcon(":/images/settings.png");
    QIcon settings_inverted = QIcon(":/images/settings_inverted.png");
    QIcon book = QIcon(":/images/book.png");
    QIcon book_inverted = QIcon(":/images/book_inverted.png");
    QIcon sliders = QIcon(":/images/sliders.png");
    QIcon sliders_inverted = QIcon(":/images/sliders_inverted.png");
    QIcon help = QIcon(":/images/help.png");
    QIcon help_inverted = QIcon(":/images/help_inverted.png");

    // Set icons for each tab
#ifdef Q_OS_MAC
    if (use_dark) {
        _actions[0]->setIcon(settings_inverted);
        _actions[1]->setIcon(book_inverted);
        _actions[2]->setIcon(sliders_inverted);
        _actions[3]->setIcon(help_inverted);
    } else {
        _actions[0]->setIcon(index == 0 ? settings_inverted : settings);
        _actions[1]->setIcon(index == 1 ? book_inverted : book);
        _actions[2]->setIcon(index == 2 ? sliders_inverted : sliders);
        _actions[3]->setIcon(index == 3 ? help_inverted : help);
    }
#else
    _actions[0]->setIcon(index == 0 ? settings_inverted : settings);
    _actions[1]->setIcon(index == 1 ? book_inverted : book);
    _actions[2]->setIcon(index == 2 ? sliders_inverted : sliders);
    _actions[3]->setIcon(index == 3 ? help_inverted : help);
#endif
}

void SettingsWindow::openTab(int tabIndex)
{
#if defined(Q_OS_DARWIN)
    // Set the style to match whether the user started dark mode
    if (!system("defaults read -g AppleInterfaceStyle")) {
        setButtonIcon(/* use_dark = */true, tabIndex);
    } else {
        setButtonIcon(/* use_dark = */false, tabIndex);
    }
#else
    setButtonIcon(/* use_dark = */false, tabIndex);
#endif

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

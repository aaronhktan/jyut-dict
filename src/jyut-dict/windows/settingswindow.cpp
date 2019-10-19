#include "settingswindow.h"

#include "components/advancedtab.h"
#include "components/contacttab.h"
#include "components/dictionarytab.h"
#include "components/settingstab.h"
#include "logic/utils/utils.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#endif
#ifdef Q_OS_WIN
#include "logic/utils/utils_windows.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QActionGroup>
#include <QGuiApplication>
#include <QPalette>
#include <QTimer>

SettingsWindow::SettingsWindow(std::shared_ptr<SQLDatabaseManager> manager,
                               QWidget *parent)
    : QMainWindow{nullptr, Qt::Window},
      _parent{parent}
{
    _manager = manager;

    _analytics = new Analytics{this};
    _analytics->sendScreenview("Settings");

    setupUI();
    translateUI();

    _toolButtons[0]->click();

    resize(sizeHint());
    layout()->setSizeConstraint(QLayout::SetFixedSize);
    move(parent->x() + (parent->width() - sizeHint().width()) / 2,
         parent->y() + (parent->height() - sizeHint().height()) / 2);

    connect(qApp,
            &QGuiApplication::applicationStateChanged,
            this,
            &SettingsWindow::paintWithApplicationState);

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
        setStyle(Utils::isDarkMode());
    }
#endif
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QMainWindow::changeEvent(event);
}

void SettingsWindow::setupUI()
{
    _contentStackedWidget = new QStackedWidget{this};
    _toolBar = new QToolBar{this};
#ifdef Q_OS_WIN
    //_toolBar->setStyleSheet("QToolBar {"
    //                        "   background-color: white;"
    //                        "}");
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

    SettingsTab *generalTab = new SettingsTab{this};
    _contentStackedWidget->addWidget(generalTab);

    DictionaryTab *dictionaryTab = new DictionaryTab{_manager, this};
    _contentStackedWidget->addWidget(dictionaryTab);

    AdvancedTab *advancedTab = new AdvancedTab{this};
    _contentStackedWidget->addWidget(advancedTab);

    ContactTab *contactTab = new ContactTab{this};

    _contentStackedWidget->addWidget(contactTab);

    setCentralWidget(_contentStackedWidget);

    // Customize the look of the toolbar to fit in better with platform styles
#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
    setStyle(Utils::isDarkMode());
#else
    setStyle(/* use_dark = */false);
#endif
}

void SettingsWindow::translateUI()
{
    // Set property so styling automatically changes
    setProperty("isHan", Settings::isCurrentLocaleHan());

    QList<QToolButton *> buttons = this->findChildren<QToolButton *>();
    for (auto button : buttons) {
        button->setProperty("isHan", Settings::isCurrentLocaleHan());
        button->style()->unpolish(button);
        button->style()->polish(button);
    }

    _actions[0]->setText(tr("General"));
    _actions[1]->setText(tr("Dictionaries"));
    _actions[2]->setText(tr("Advanced"));
    _actions[3]->setText(tr("Contact"));

#ifdef Q_OS_MAC
    setWindowTitle(tr("Preferences"));
#else
    setWindowTitle(tr("Settings"));
#endif
}

void SettingsWindow::setStyle(bool use_dark)
{
    // Set background color of tabs in toolbar
    QColor selectedBackgroundColour;
    QColor currentTextColour;
    QColor otherTextColour;
    if (QGuiApplication::applicationState() == Qt::ApplicationInactive) {
#ifdef Q_OS_MAC
        selectedBackgroundColour = QGuiApplication::palette()
            .color(QPalette::Inactive, QPalette::Highlight);
#else
        selectedBackgroundColour = use_dark ? Utils::LIST_ITEM_INACTIVE_COLOUR_DARK
                                            : Utils::LIST_ITEM_INACTIVE_COLOUR_LIGHT;
#endif
        currentTextColour = use_dark
                                ? Utils::TOOLBAR_TEXT_INACTIVE_COLOUR_DARK
                                : Utils::TOOLBAR_TEXT_INACTIVE_COLOUR_LIGHT;
        otherTextColour = use_dark ? Utils::TOOLBAR_TEXT_INACTIVE_COLOUR_DARK
                                   : Utils::TOOLBAR_TEXT_INACTIVE_COLOUR_LIGHT;
    } else {
#ifdef Q_OS_MAC
        selectedBackgroundColour = Utils::getAppleControlAccentColor();
#else
        selectedBackgroundColour = use_dark ? Utils::LIST_ITEM_ACTIVE_COLOUR_DARK
                                            : Utils::LIST_ITEM_ACTIVE_COLOUR_LIGHT;
#endif
        currentTextColour = Utils::getContrastingColour(selectedBackgroundColour);
#ifdef Q_OS_MAC
        otherTextColour = QGuiApplication::palette().color(QPalette::Active,
                                                           QPalette::Text);
#else
        otherTextColour = use_dark
                              ? Utils::TOOLBAR_TEXT_NOT_FOCUSED_COLOUR_DARK
                              : Utils::TOOLBAR_TEXT_NOT_FOCUSED_COLOUR_LIGHT;
#endif
    }

#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
    QString style;
    if (use_dark) {
        style = "QToolButton[isHan=\"true\"] { "
                "   border-radius: 2px; "
                "   color: %4; "
                "   font-size: %2px; "
                "   margin: 0px; "
                "}"
                ""
                "QToolButton { "
                "   border-top-left-radius: 4px; "
                "   border-top-right-radius: 4px; "
                "   color: %4; "
                "   font-size: 10px; "
                "   margin: 0px; "
                "}"
                " "
                "QToolButton:checked { "
                "   background-color: %1; "
                "   border-top-left-radius: 4px; "
                "   border-top-right-radius: 4px; "
                "   color: %3; "
                "   margin: 0px; "
                "}";
    } else {
        style = "QToolButton[isHan=\"true\"] { "
                "   border-radius: 2px; "
                "   color: %4; "
                "   font-size: %2px; "
                "   margin: 0px; "
                "}"
                ""
                "QToolButton { "
                "   border-top-left-radius: 4px; "
                "   border-top-right-radius: 4px; "
                "   color: %4; "
                "   font-size: 10px; "
                "   margin: 0px; "
                "}"
                " "
                "QToolButton:checked { "
                "   background-color: %1; "
                "   border-top-left-radius: 4px; "
                "   border-top-right-radius: 4px; "
                "   color: %3; "
                "   margin: 0px; "
                "}";
    }
#else
    QString style{"QToolButton[isHan=\"true\"] { "
                  "   border-radius: 2px; "
                  "   color: %4; "
                  "   font-size: %2px; "
                  "   margin: 0px; "
                  "}"
                  " "
                  "QToolButton { "
                  "   border-radius: 2px; "
                  "   color: %4; "
                  "   font-size: 10px; "
                  "   margin: 0px; "
                  "}"
                  " "
                  "QToolButton:checked { "
                  "   border-radius: 2px; "
                  "   background-color: %1; "
                  "   color: %3; "
                  "   margin: 0px; "
                  "}"};
#endif
    setStyleSheet(
        style.arg(selectedBackgroundColour.name(),
                  "13",
                  currentTextColour.name(),
                  otherTextColour.name()));
    setButtonIcon(use_dark, _contentStackedWidget->currentIndex());
}

void SettingsWindow::setButtonIcon(bool use_dark, int index)
{
    // For these images, export as 96px width, and center on 120px canvas.
    QIcon settings = QIcon(":/images/settings.png");
    QIcon settings_inverted = QIcon(":/images/settings_inverted.png");
    QIcon settings_disabled = QIcon(":/images/settings_disabled.png");
    QIcon book = QIcon(":/images/book.png");
    QIcon book_inverted = QIcon(":/images/book_inverted.png");
    QIcon book_disabled = QIcon(":/images/book_disabled.png");
    QIcon sliders = QIcon(":/images/sliders.png");
    QIcon sliders_inverted = QIcon(":/images/sliders_inverted.png");
    QIcon sliders_disabled = QIcon(":/images/sliders_disabled.png");
    QIcon help = QIcon(":/images/help.png");
    QIcon help_inverted = QIcon(":/images/help_inverted.png");
    QIcon help_disabled = QIcon(":/images/help_disabled.png");

    // Set icons for each tab
    if (QGuiApplication::applicationState() == Qt::ApplicationInactive) {
        _actions[0]->setIcon(settings_disabled);
        _actions[1]->setIcon(book_disabled);
        _actions[2]->setIcon(sliders_disabled);
        _actions[3]->setIcon(help_disabled);
    } else {
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
    }

    QList<QToolButton *> buttons = this->findChildren<QToolButton *>();
    for (auto button : buttons) {
        button->style()->unpolish(button);
        button->style()->polish(button);
    }
}

void SettingsWindow::openTab(int tabIndex)
{
#if defined(Q_OS_DARWIN) || defined(Q_OS_WIN)
    // Set the style to match whether the user started dark mode
    setButtonIcon(Utils::isDarkMode(), tabIndex);
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

    _analytics->sendScreenview(
        _actions[static_cast<std::vector<QAction *>::size_type>(tabIndex)]
            ->text()
            .toStdString());
}

void SettingsWindow::paintWithApplicationState(Qt::ApplicationState state)
{
#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
    setStyle(Utils::isDarkMode());
#else
    setStyle(/* use_dark = */ false);
#endif
}

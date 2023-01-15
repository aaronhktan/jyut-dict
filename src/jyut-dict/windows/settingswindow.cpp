#include "settingswindow.h"

#include "components/settings/advancedtab.h"
#include "components/settings/contacttab.h"
#include "components/settings/dictionarytab.h"
#include "components/settings/settingstab.h"
#include "components/settings/texttab.h"
#include "logic/utils/utils.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
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

void SettingsWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, this, [=]() { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QMainWindow::changeEvent(event);
}

void SettingsWindow::setupUI()
{
    _contentStackedWidget = new QStackedWidget{this};
    _toolBar = new QToolBar{this};
    addToolBar(_toolBar);
    setUnifiedTitleAndToolBarOnMac(true);
    _toolBar->setMovable(false);
    _toolBar->setContextMenuPolicy(Qt::PreventContextMenu);

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

    TextTab *textTab = new TextTab{this};
    _contentStackedWidget->addWidget(textTab);

    DictionaryTab *dictionaryTab = new DictionaryTab{_manager, this};
    _contentStackedWidget->addWidget(dictionaryTab);

    AdvancedTab *advancedTab = new AdvancedTab{this};
    _contentStackedWidget->addWidget(advancedTab);

    ContactTab *contactTab = new ContactTab{this};
    _contentStackedWidget->addWidget(contactTab);

    setCentralWidget(_contentStackedWidget);

    connect(advancedTab,
            &AdvancedTab::settingsReset,
            generalTab,
            &SettingsTab::resetSettings);

    connect(advancedTab,
            &AdvancedTab::settingsReset,
            textTab,
            &TextTab::resetSettings);

    // Customize the look of the toolbar to fit in better with platform styles
    setStyle(Utils::isDarkMode());
}

void SettingsWindow::translateUI()
{
    // Set property so styling automatically changes
    setProperty("isHan", Settings::isCurrentLocaleHan());

    QList<QToolButton *> buttons = this->findChildren<QToolButton *>();
    foreach (const auto & button, buttons) {
        button->setProperty("isHan", Settings::isCurrentLocaleHan());
        button->style()->unpolish(button);
        button->style()->polish(button);
    }

    _actions[0]->setText(tr("General"));
    _actions[1]->setText(tr("Text"));
    _actions[2]->setText(tr("Dictionaries"));
    _actions[3]->setText(tr("Advanced"));
    _actions[4]->setText(tr("Contact"));

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
        selectedBackgroundColour = QGuiApplication::palette()
            .color(QPalette::Inactive, QPalette::Highlight);
        currentTextColour
            = use_dark ? QColor{TOOLBAR_TEXT_INACTIVE_COLOUR_DARK_R,
                                TOOLBAR_TEXT_INACTIVE_COLOUR_DARK_G,
                                TOOLBAR_TEXT_INACTIVE_COLOUR_DARK_B}
                       : QColor{TOOLBAR_TEXT_INACTIVE_COLOUR_LIGHT_R,
                                TOOLBAR_TEXT_INACTIVE_COLOUR_LIGHT_G,
                                TOOLBAR_TEXT_INACTIVE_COLOUR_LIGHT_B};
        otherTextColour
            = use_dark ? QColor{TOOLBAR_TEXT_INACTIVE_COLOUR_DARK_R,
                                TOOLBAR_TEXT_INACTIVE_COLOUR_DARK_G,
                                TOOLBAR_TEXT_INACTIVE_COLOUR_DARK_B}
                       : QColor{TOOLBAR_TEXT_INACTIVE_COLOUR_LIGHT_R,
                                TOOLBAR_TEXT_INACTIVE_COLOUR_LIGHT_G,
                                TOOLBAR_TEXT_INACTIVE_COLOUR_LIGHT_B};
    } else {
#ifdef Q_OS_MAC
        selectedBackgroundColour = Utils::getAppleControlAccentColor();
#else
        selectedBackgroundColour
            = use_dark ? QColor{LIST_ITEM_ACTIVE_COLOUR_DARK_R,
                                LIST_ITEM_ACTIVE_COLOUR_DARK_G,
                                LIST_ITEM_ACTIVE_COLOUR_DARK_B}
                       : QColor{LIST_ITEM_ACTIVE_COLOUR_LIGHT_R,
                                LIST_ITEM_ACTIVE_COLOUR_LIGHT_G,
                                LIST_ITEM_ACTIVE_COLOUR_LIGHT_B};
#endif
        currentTextColour = Utils::getContrastingColour(selectedBackgroundColour);
#ifdef Q_OS_MAC
        otherTextColour = QGuiApplication::palette().color(QPalette::Active,
                                                           QPalette::Text);
#else
        otherTextColour
            = use_dark ? QColor{TOOLBAR_TEXT_NOT_FOCUSED_COLOUR_DARK_R,
                                TOOLBAR_TEXT_NOT_FOCUSED_COLOUR_DARK_G,
                                TOOLBAR_TEXT_NOT_FOCUSED_COLOUR_DARK_B}
                       : QColor{TOOLBAR_TEXT_NOT_FOCUSED_COLOUR_LIGHT_R,
                                TOOLBAR_TEXT_NOT_FOCUSED_COLOUR_LIGHT_G,
                                TOOLBAR_TEXT_NOT_FOCUSED_COLOUR_LIGHT_B};
#endif
    }

#ifdef Q_OS_MAC
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

    // Customize the look of the toolbar
#if defined(Q_OS_WIN)
    if (use_dark) {
        _toolBar->setStyleSheet("QToolBar {"
                                "   background-color: black; "
                                "   border-top: 1px solid black; "
                                "}");
    } else {
        _toolBar->setStyleSheet("QToolBar {"
                                "   background-color: white;"
                                "}");
    }
#elif defined(Q_OS_LINUX)
    if (Utils::isDarkMode()) {
        _toolBar->setStyleSheet("QToolBar { "
                                "   background-color: palette(alternate-base); "
                                "   border-bottom: 1px solid palette(window); "
                                "   padding-bottom: 3px; "
                                "   padding-top: 3px; "
                                "} ");
    } else {
        _toolBar->setStyleSheet("QToolBar { "
                                "   background-color: palette(window); "
                                "   border-bottom: 1px solid palette(alternate-base); "
                                "   padding-bottom: 3px; "
                                "   padding-top: 3px; "
                                "} ");
    }
#endif
}

void SettingsWindow::setButtonIcon(bool use_dark, int index)
{
    // For these images, export using Inkscape drawing @ 96px image size width,
    // then use Gimp to center on 120px canvas with a transparent background.
    // To generate inverted, use Colors -> Invert in Gimp.
    // To generate disabled, use Colors -> Brightness-Contrast to -60 in Gimp.
    QIcon settings = QIcon(":/images/settings.png");
    QIcon settings_inverted = QIcon(":/images/settings_inverted.png");
    QIcon settings_disabled = QIcon(":/images/settings_disabled.png");
    QIcon text = QIcon(":/images/text.png");
    QIcon text_inverted = QIcon(":/images/text_inverted.png");
    QIcon text_disabled = QIcon(":/images/text_disabled.png");
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
        _actions[1]->setIcon(text_disabled);
        _actions[2]->setIcon(book_disabled);
        _actions[3]->setIcon(sliders_disabled);
        _actions[4]->setIcon(help_disabled);
    } else {
        if (use_dark) {
            _actions[0]->setIcon(settings_inverted);
            _actions[1]->setIcon(text_inverted);
            _actions[2]->setIcon(book_inverted);
            _actions[3]->setIcon(sliders_inverted);
            _actions[4]->setIcon(help_inverted);
        } else {
            _actions[0]->setIcon(index == 0 ? settings_inverted : settings);
            _actions[1]->setIcon(index == 1 ? text_inverted : text);
            _actions[2]->setIcon(index == 1 ? book_inverted : book);
            _actions[3]->setIcon(index == 2 ? sliders_inverted : sliders);
            _actions[4]->setIcon(index == 3 ? help_inverted : help);
        }
    }

    QList<QToolButton *> buttons = this->findChildren<QToolButton *>();
    foreach (const auto & button, buttons) {
        button->style()->unpolish(button);
        button->style()->polish(button);
    }
}

void SettingsWindow::openTab(int tabIndex)
{
    // Set the style to match whether the user started dark mode
    setButtonIcon(Utils::isDarkMode(), tabIndex);

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

void SettingsWindow::paintWithApplicationState(Qt::ApplicationState state)
{
    (void) (state);
    setStyle(Utils::isDarkMode());
}

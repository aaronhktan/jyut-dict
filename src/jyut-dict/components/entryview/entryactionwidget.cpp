#include "entryactionwidget.h"

#include "logic/settings/settings.h"
#include "logic/settings/settingsutils.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QFileDialog>

EntryActionWidget::EntryActionWidget(std::shared_ptr<SQLUserDataUtils> sqlUserUtils,
                                     QWidget *parent)
    : QWidget(parent)
    , _sqlUserUtils{sqlUserUtils}
{
    _settings = Settings::getSettings(this);

    _sqlUserUtils->registerObserver(this);

    setupUI();
}

EntryActionWidget::~EntryActionWidget(void)
{
    _sqlUserUtils->deregisterObserver(this);
}

void EntryActionWidget::callback(bool entryExists, const Entry &entry)
{
    emit callbackTriggered(entryExists, entry);
}

void EntryActionWidget::changeEvent(QEvent *event)
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
    QWidget::changeEvent(event);
}

void EntryActionWidget::setEntry(const Entry &entry)
{
    _entry = entry;

    disconnect(this, nullptr, this, nullptr);
    connect(this,
            &EntryActionWidget::callbackTriggered,
            this,
            [=](bool entryExists, const Entry existenceEntry) {
                if (entry != existenceEntry) {
                    return;
                }
#ifdef Q_OS_MAC
                // On other platforms, this causes issues with scrolling.
                // Restrict to macOS.
                setVisible(false);
#endif
                _bookmarkButton->setProperty("saved",
                                             QVariant::fromValue(entryExists));
                refreshBookmarkButton();
                setVisible(true);
            });

    _sqlUserUtils->checkIfEntryHasBeenFavourited(entry);
}

void EntryActionWidget::setupUI(void)
{
    _bookmarkButton = new QPushButton{this};
    _bookmarkButton->setVisible(false);

    _shareButton = new QPushButton{this};
    _shareButton->setVisible(false);
    connect(_shareButton,
            &QPushButton::clicked,
            this,
            &EntryActionWidget::shareAction);

    _openInNewWindowButton = new QPushButton{this};
    _openInNewWindowButton->setVisible(false);
    connect(_openInNewWindowButton,
            &QPushButton::clicked,
            this,
            &EntryActionWidget::openInNewWindow);

    _magnifyButton = new QPushButton{this};
    _magnifyButton->setVisible(false);
    connect(_magnifyButton,
            &QPushButton::clicked,
            this,
            &EntryActionWidget::openMagnifyWindow);

    _layout = new FlowLayout{this};
    _layout->setContentsMargins(0, 5, 0, 15);
    _layout->setSpacing(5);
    _layout->addWidget(_bookmarkButton);
    _layout->addWidget(_shareButton);
    _layout->addWidget(_openInNewWindowButton);
    _layout->addWidget(_magnifyButton);

    setStyle(Utils::isDarkMode());
    translateUI();

    setVisible(false);
}

void EntryActionWidget::translateUI(void)
{
    _bookmarkButton->setText(
        _bookmarkButton->property("saved").toBool() ? tr("Saved!") : tr("Save"));
    _shareButton->setText(tr("Share"));
    _openInNewWindowButton->setText(tr("Open in New Window"));
    _magnifyButton->setText(tr("Magnify"));
}

void EntryActionWidget::setStyle(bool use_dark)
{
    QColor textColour = use_dark ? QColor{LABEL_TEXT_COLOUR_DARK_R,
                                          LABEL_TEXT_COLOUR_DARK_G,
                                          LABEL_TEXT_COLOUR_DARK_B}
                                 : QColor{LABEL_TEXT_COLOUR_LIGHT_R,
                                          LABEL_TEXT_COLOUR_LIGHT_G,
                                          LABEL_TEXT_COLOUR_LIGHT_B};
    QColor borderColour = use_dark ? QColor{CONTENT_BACKGROUND_COLOUR_DARK_R,
                                            CONTENT_BACKGROUND_COLOUR_DARK_G,
                                            CONTENT_BACKGROUND_COLOUR_DARK_B}
                                   : QColor{CONTENT_BACKGROUND_COLOUR_LIGHT_R,
                                            CONTENT_BACKGROUND_COLOUR_LIGHT_G,
                                            CONTENT_BACKGROUND_COLOUR_LIGHT_B};

    int interfaceSize = static_cast<int>(
        _settings
            ->value("Interface/size",
                    QVariant::fromValue(Settings::InterfaceSize::NORMAL))
            .value<Settings::InterfaceSize>());
    int bodyFontSize = Settings::bodyFontSize.at(
        static_cast<unsigned long>(interfaceSize - 1));
    int borderRadius = static_cast<int>(bodyFontSize * 1);
    int padding = bodyFontSize / 6;
    int paddingHorizontal = bodyFontSize / 3;
    QString styleSheet = "QPushButton { "
                         "   background-color: palette(base); "
                         "   border: 2px solid %1; "
                         "   border-radius: %2px; "
                         "   color: %3; "
                         "   font-size: %4px; "
                         "   icon-size: %4px; "
                         "   padding: %5px; "
                         "   padding-left: %6px; "
                         "   padding-right: %6px; "
                         "} "
                         ""
                         "QPushButton:hover { "
                         "   background-color: %1; "
                         "   border: 2px solid %1; "
                         "   border-radius: %2px; "
                         "   color: %3; "
                         "   font-size: %4px; "
                         "   icon-size: %4px; "
                         "   padding: %5px; "
                         "   padding-left: %6px; "
                         "   padding-right: %6px; "
                         "} ";

    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (const auto &button, buttons) {
        button->setStyleSheet(styleSheet.arg(borderColour.name())
                                  .arg(borderRadius)
                                  .arg(textColour.name())
                                  .arg(bodyFontSize)
                                  .arg(padding)
                                  .arg(paddingHorizontal));
        button->setMinimumHeight(borderRadius * 2);

#ifdef Q_OS_MAC
        // Hack to get around weird button sizing issues when switching styles
        button->setVisible(false);
        button->setVisible(true);
#endif
    }

    if (_bookmarkButton->property("saved").toBool()) {
        _bookmarkButton->setIcon(
            QIcon{use_dark ? ":/images/star_inverted_filled.png" : ":/images/star_filled.png"});
    } else {
        _bookmarkButton->setIcon(
            QIcon{use_dark ? ":/images/star_inverted.png" : ":/images/star.png"});
    }
    _shareButton->setIcon(
        QIcon{use_dark ? ":/images/share_inverted.png" : ":/images/share.png"});
    _openInNewWindowButton->setIcon(
        QIcon{use_dark ? ":/images/external_link_inverted.png"
                       : ":/images/external_link.png"});
    _magnifyButton->setIcon(QIcon{use_dark ? ":/images/zoom_in_inverted.png"
                                           : ":/images/zoom_in.png"});
}

void EntryActionWidget::refreshBookmarkButton(void)
{
    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (const auto &button, buttons) {
        button->setVisible(true);
    }

    setStyle(Utils::isDarkMode());
    translateUI();

    disconnect(_bookmarkButton, nullptr, nullptr, nullptr);
    if (!_bookmarkButton->property("saved").toBool()) {
        QObject::connect(_bookmarkButton, &QPushButton::clicked, this, [=]() {
            addEntryToFavourites(_entry);
        });
    } else {
        QObject::connect(_bookmarkButton, &QPushButton::clicked, this, [=]() {
            removeEntryFromFavourites(_entry);
        });
    }
}

void EntryActionWidget::addEntryToFavourites(const Entry &entry)
{
    _sqlUserUtils->favouriteEntry(entry);
}

void EntryActionWidget::removeEntryFromFavourites(const Entry &entry)
{
    _sqlUserUtils->unfavouriteEntry(entry);
}

void EntryActionWidget::shareAction(void)
{
    // This prevents the button from showing up as "pressed" in the screenshot
    _shareButton->setAttribute(Qt::WA_UnderMouse, false);

    QPixmap map{parentWidget()->grab()};
    QFileDialog *_fileDialog = new QFileDialog{this};
    _fileDialog->setAcceptMode(QFileDialog::AcceptSave);

    QString fileName = _fileDialog
                           ->getSaveFileName(this,
                                             tr("Path to save exported image"),
                                             QDir::homePath());
    if (!fileName.toStdString().empty()) {
        if (!fileName.endsWith(".png")) {
            fileName = fileName + ".png";
        }
        if (QFile::exists(fileName)) {
            QFile::remove(fileName);
        }
        map.save(fileName, "png");
    }
}

void EntryActionWidget::favouriteCurrentEntryRequested(void)
{
    if (_bookmarkButton->isVisible()) {
        _bookmarkButton->click();
    }
}

void EntryActionWidget::shareCurrentEntryRequested(void)
{
    if (_shareButton->isVisible()) {
        shareAction();
    }
}

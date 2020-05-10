#include "entryactionwidget.h"

#include "components/favouritewindow/favouritesplitter.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QFileDialog>

EntryActionWidget::EntryActionWidget(std::shared_ptr<SQLUserDataUtils> sqlUserUtils,
                                     QWidget *parent)
    : QWidget(parent)
    , _sqlUserUtils{sqlUserUtils}
{
    _sqlUserUtils->registerObserver(this);

    setupUI();
}

EntryActionWidget::~EntryActionWidget(void)
{
    _sqlUserUtils->deregisterObserver(this);
}

void EntryActionWidget::callback(bool entryExists, Entry entry)
{
    emit callbackTriggered(entryExists, entry);
}

void EntryActionWidget::changeEvent(QEvent *event)
{
#if defined(Q_OS_DARWIN)
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, [=]() { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
#endif
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QWidget::changeEvent(event);
}

void EntryActionWidget::setEntry(const Entry entry)
{
    _sqlUserUtils->checkIfEntryHasBeenFavourited(entry);
    _entry = entry;

    disconnect(this, nullptr, nullptr, nullptr);
    connect(this,
            &EntryActionWidget::callbackTriggered,
            this,
            [=](bool entryExists, const Entry existenceEntry) {
                if (entry != existenceEntry) {
                    return;
                }
                setVisible(false);
                _bookmarkButton->setProperty("saved",
                                             QVariant::fromValue(entryExists));
                refreshBookmarkButton();
                setVisible(true);
    });
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

    _layout = new QHBoxLayout{this};
    _layout->setContentsMargins(0, 5, 0, 15);
    _layout->setSpacing(5);
    _layout->addWidget(_bookmarkButton);
    _layout->addWidget(_shareButton);
    _layout->addStretch(0);

#ifdef Q_OS_MAC
    setStyle(Utils::isDarkMode());
#else
    setStyle(/* use_dark = */ false);
#endif
    translateUI();

    setVisible(false);
}

void EntryActionWidget::translateUI(void)
{
    _bookmarkButton->setText(
        _bookmarkButton->property("saved").toBool() ? "Saved!" : "Save");
    _shareButton->setText(tr("Share"));
}

void EntryActionWidget::setStyle(bool use_dark)
{
    QColor textColour = use_dark ? QColor{LABEL_TEXT_COLOUR_DARK_R,
                                          LABEL_TEXT_COLOUR_DARK_G,
                                          LABEL_TEXT_COLOUR_DARK_B}
                                 : QColor{LABEL_TEXT_COLOUR_LIGHT_R,
                                          LABEL_TEXT_COLOUR_LIGHT_G,
                                          LABEL_TEXT_COLOUR_LIGHT_B};
    QColor borderColour = use_dark ? textColour.darker(300)
                                   : textColour.lighter(200);
    QString styleSheet = "QPushButton { border: 2px solid %1; "
                         "border-radius: 13px; "
                         "color: %2; "
                         "font-size: 12px; "
                         "padding: 3px; "
                         "padding-right: 6px; } ";
    _bookmarkButton->setStyleSheet(
        styleSheet.arg(borderColour.name(), textColour.name()));
    _shareButton->setStyleSheet(
        styleSheet.arg(borderColour.name(), textColour.name()));

    _bookmarkButton->setIcon(
        QIcon{use_dark ? ":/images/star_inverted.png" : ":/images/star.png"});
    _shareButton->setIcon(
        QIcon{use_dark ? ":/images/share_inverted.png" : ":/images/share.png"});

    // Hack to get around weird button sizing issues when switching styles
    _bookmarkButton->setVisible(false);
    _shareButton->setVisible(false);
    _bookmarkButton->setVisible(true);
    _shareButton->setVisible(true);
}

void EntryActionWidget::refreshBookmarkButton(void)
{
    _bookmarkButton->setVisible(true);
    _shareButton->setVisible(true);
#ifdef Q_OS_MAC
    setStyle(Utils::isDarkMode());
#else
    setStyle(/* use_dark = */ false);
#endif
    translateUI();

    disconnect(_bookmarkButton, nullptr, nullptr, nullptr);
    if (!_bookmarkButton->property("saved").toBool()) {
        QObject::connect(_bookmarkButton, &QPushButton::clicked, this, [=]() {
            addEntryToFavourites(_entry);
            _bookmarkButton->setProperty("saved", QVariant::fromValue(true));
            refreshBookmarkButton();
        });
    } else {
        QObject::connect(_bookmarkButton, &QPushButton::clicked, this, [=]() {
            removeEntryFromFavourites(_entry);
            _bookmarkButton->setProperty("saved", QVariant::fromValue(false));
            refreshBookmarkButton();
        });
    }
}

void EntryActionWidget::addEntryToFavourites(Entry entry)
{
    _sqlUserUtils->favouriteEntry(entry);
}

void EntryActionWidget::removeEntryFromFavourites(Entry entry)
{
    _sqlUserUtils->unfavouriteEntry(entry);
}

void EntryActionWidget::shareAction(void)
{
    QPixmap map{parentWidget()->grab()};
    QFileDialog *_fileDialog = new QFileDialog{this};
    _fileDialog->setAcceptMode(QFileDialog::AcceptSave);

    QString fileName = _fileDialog
                           ->getSaveFileName(this,
                                             tr("Path to save exported image"),
                                             QDir::homePath());
    if (!fileName.toStdString().empty()) {
        map.save(fileName, "png");
    }
}

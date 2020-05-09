#include "entryactionwidget.h"

#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#endif
#include "logic/utils/utils_qt.h"

EntryActionWidget::EntryActionWidget(std::shared_ptr<SQLDatabaseManager> manager,
                                     QWidget *parent)
    : QWidget(parent)
    , _manager{manager}
{
    _utils = std::make_unique<SQLUserDataUtils>(_manager);
    _utils->registerObserver(this);

    setupUI();
}

void EntryActionWidget::callback(bool entryExists)
{
    emit callbackTriggered(entryExists);
}

void EntryActionWidget::setEntry(Entry entry)
{
    _bookmarkButton->setVisible(false);
    _utils->checkIfEntryHasBeenFavourited(entry);
    _entry = entry;

    disconnect(this, nullptr, nullptr, nullptr);
    connect(this,
            &EntryActionWidget::callbackTriggered,
            this,
            [=](bool entryExists) {
                _bookmarkButton->setProperty("saved",
                                             QVariant::fromValue(entryExists));
                _bookmarkButton->setText(entryExists ? tr("Saved!")
                                                     : tr("Save"));
                _bookmarkButton->setVisible(true);
                _shareButton->setVisible(true);
#ifdef Q_OS_MAC
        setStyle(Utils::isDarkMode());
#else
        setStyle(/* use_dark = */false);
#endif
    });
}

void EntryActionWidget::setupUI(void)
{
    _bookmarkButton = new QPushButton{this};
    _bookmarkButton->setVisible(false);

    _shareButton = new QPushButton{this};
    _shareButton->setVisible(false);

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
                                   : textColour.lighter(100);
    QString styleSheet = "QPushButton { border: 1px solid %1; "
                         "border-radius: 12px; "
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
        QIcon{use_dark ? ":/images/share_inverted.png" : ":/images/star.png"});
}

void EntryActionWidget::addEntryToFavourites(Entry entry)
{
    (void) (entry);
}

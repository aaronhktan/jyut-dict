#include "defaultdialog.h"

#include "logic/settings/settings.h"
#include "logic/settings/settingsutils.h"
#include "logic/strings/strings.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#endif

#include <QCoreApplication>
#include <QAbstractButton>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QStyle>

DefaultDialog::DefaultDialog(const QString &reason,
                             const QString &description,
                             QWidget *parent)
    : QMessageBox{parent}
{
    _settings = Settings::getSettings();

    setupUI(reason, description);
    translateUI();

#ifdef Q_OS_MAC
    // Set the style to match whether the user started dark mode
    setStyle(Utils::isDarkMode());
#else
    setStyle(false);
#endif
}

void DefaultDialog::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QWidget::changeEvent(event);
}

void DefaultDialog::setWidth(int width)
{
    // Setting minimum width also doesn't work, so use this
    // workaround to set a width.
    // Must be called after buttons are set!
    QSpacerItem *horizontalSpacer = new QSpacerItem(width,
                                                    0,
                                                    QSizePolicy::Minimum,
                                                    QSizePolicy::Minimum);
    QGridLayout *layout = static_cast<QGridLayout *>(this->layout());
    layout->addItem(horizontalSpacer,
                    layout->rowCount(),
                    0,
                    1,
                    layout->columnCount());
}

void DefaultDialog::setupUI(const QString &reason, const QString &description)
{
    Qt::WindowFlags flags = windowFlags() | Qt::CustomizeWindowHint;
    flags &= ~(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint
               | Qt::WindowFullscreenButtonHint);
    setWindowFlags(flags);
    setInformativeText(reason);
    setDetailedText(description);
    setIcon(QMessageBox::Warning);
#ifdef Q_OS_WIN
    setWindowTitle(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                               Strings::PRODUCT_NAME));
#elif defined(Q_OS_LINUX)
    setWindowTitle(" ");
#endif

    QList<QLabel *> labels = this->findChildren<QLabel *>();
    foreach (const auto &label, labels) {
        label->setTextInteractionFlags(Qt::NoTextInteraction);
    }

    setWidth(400);
    deselectButtons();
}

void DefaultDialog::translateUI(void)
{
    // Set property so styling automatically changes
    setProperty("isHan", Settings::isCurrentLocaleHan());

    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (const auto & button, buttons) {
        button->setProperty("isHan", Settings::isCurrentLocaleHan());
        button->style()->unpolish(button);
        button->style()->polish(button);
    }
}

void DefaultDialog::setStyle(bool use_dark)
{
    (void) (use_dark);
    int interfaceSize = static_cast<int>(
        _settings
            ->value("Interface/size",
                    QVariant::fromValue(Settings::InterfaceSize::NORMAL))
            .value<Settings::InterfaceSize>());
    int bodyFontSize = Settings::bodyFontSize.at(
        static_cast<unsigned long>(interfaceSize - 1));
    int bodyFontSizeHan = Settings::bodyFontSizeHan.at(
        static_cast<unsigned long>(interfaceSize - 1));

#ifdef Q_OS_MAC
    QString style{"QLabel[isHan=\"true\"] { "
                  "   font-size: %1px; "
                  "} "
                  " "
                  "QLabel { "
                  "   font-size: %2px; "
                  "} "
                  " "
                  "QRadioButton { "
                  "   font-size: %2px; "
                  "} "
                  " "
                  "QPushButton[isHan=\"true\"] { "
                  "   font-size: %1px; "
                  //// QPushButton falls back to Fusion style on macOS when the
                  //// height exceeds 16px. Set the maximum size to 16px.
                  "   height: 16px; "
                  "} "
                  " "
                  "QPushButton { "
                  "   font-size: %2px; "
                  "   height: 16px; "
                  "} "};
#else
    QString style{"QLabel[isHan=\"true\"] { "
                  "   font-size: %1px; "
                  "} "
                  " "
                  "QLabel { "
                  "   font-size: %2px; "
                  "} "
                  " "
                  "QPushButton[isHan=\"true\"] { "
                  "   font-size: %1px; "
                  "   height: 16px; "
                  "} "
                  " "
                  "QPushButton { "
                  "   font-size: %2px; "
                  "   height: 16px; "
                  "} "};
#endif
    setStyleSheet(style.arg(std::to_string(bodyFontSizeHan).c_str(),
                            std::to_string(bodyFontSize).c_str()));
}

void DefaultDialog::deselectButtons(void)
{
    // setDefaultButton doesn't really work, so use this
    // workaround to deselect all buttons first.
    foreach (const auto & button, buttons()) {
        QPushButton *b = static_cast<QPushButton *>(button);
        b->setDown(false);
        b->setAutoDefault(false);
        b->setDefault(false);
        b->setAttribute(Qt::WA_MacShowFocusRect, 0);
    }
}

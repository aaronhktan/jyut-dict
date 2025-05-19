#include "aboutwindow.h"

#include "logic/settings/settingsutils.h"
#include "logic/strings/strings.h"
#include "logic/utils/utils.h"
#include "logic/utils/utils_qt.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined(Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif

#include <QApplication>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QPixmap>
#include <QString>
#include <QStyle>
#include <QTimer>
#include <QUrl>

AboutWindow::AboutWindow(QWidget *parent)
    : QWidget{parent, Qt::Window}
{
    setupUI();
    translateUI();

    Qt::WindowFlags flags = windowFlags() | Qt::CustomizeWindowHint
                            | Qt::WindowTitleHint | Qt::MSWindowsFixedSizeDialogHint;
    flags &= ~(Qt::WindowMinMaxButtonsHint | Qt::WindowFullscreenButtonHint);
    setWindowFlags(flags);

#ifdef Q_OS_MAC
    resize(sizeHint());
#elif defined(Q_OS_WIN)
    resize(minimumWidth(), heightForWidth(minimumWidth()));
#else
    setFixedSize(sizeHint());
#endif
    move(parent->x() + (parent->width() - sizeHint().width()) / 2,
         parent->y() + (parent->height() - sizeHint().height()) / 2);

    setAttribute(Qt::WA_DeleteOnClose);
}

void AboutWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, this, [=, this]() { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QWidget::changeEvent(event);
}

bool AboutWindow::eventFilter(QObject *object, QEvent *event)
{
    if (object == _iconLabel && event->type() == QEvent::MouseButtonPress) {
        QDesktopServices::openUrl(QUrl{Utils::WEBSITE_LINK});
    }
    return QObject::eventFilter(object, event);
}

void AboutWindow::setupUI()
{
    _windowLayout = new QGridLayout{this};

    _iconLabel = new QLabel{this};
    _iconLabel->setFixedWidth(100);
    _iconLabel->setFixedHeight(100);
    _iconLabel->setAlignment(Qt::AlignTop);
    _iconLabel->installEventFilter(this);

    QPixmap icon = QPixmap{":/images/icon.png"};
    icon.setDevicePixelRatio(devicePixelRatio());
    int iconWidth = static_cast<int>(devicePixelRatio() * _iconLabel->width());
    int iconHeight = static_cast<int>(devicePixelRatio() * _iconLabel->height());
    _iconLabel->setPixmap(icon.scaled(iconWidth,
                                      iconHeight,
                                      Qt::KeepAspectRatio,
                                      Qt::SmoothTransformation));

    _titleLabel = new QLabel{this};
    _titleLabel->setStyleSheet("QLabel { font-size: 20px; } ");
    _titleLabel->setText(
        QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::PRODUCT_NAME));

    _versionLabel = new QLabel{this};
    _versionLabel->setStyleSheet("QLabel { font-size: 10px; } ");
    _versionLabel->setText(tr("Build %1").arg(Utils::CURRENT_VERSION));

    _descriptionLabel = new QLabel{this};
    _descriptionLabel->setText(
        QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::PRODUCT_DESCRIPTION));
    _descriptionLabel->setAlignment(Qt::AlignHCenter);
    _descriptionLabel->setWordWrap(true);
    _descriptionLabel->setSizePolicy(QSizePolicy::MinimumExpanding,
                                     QSizePolicy::MinimumExpanding);

    _messageLabel = new QLabel{this};
    _messageLabel->setAlignment(Qt::AlignHCenter);
    _messageLabel->setStyleSheet("QLabel { font-size: 10px; }");
    _messageLabel->setText(
        QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::CREDITS_TEXT)
            .arg(palette().text().color().name()));
    _messageLabel->setWordWrap(true);
    connect(_messageLabel,
            &QLabel::linkActivated,
            this,
            [&](const QString &link) {
                if (link == "#") {
                    QApplication::aboutQt();
                } else {
                    QDesktopServices::openUrl(QUrl{link});
                }
            });

    _websiteButton = new QPushButton{tr("Visit website..."), this};
    connect(_websiteButton, &QPushButton::clicked, this, [&]() {
        QDesktopServices::openUrl(QUrl{Utils::WEBSITE_LINK});
    });

    _githubButton = new QPushButton{tr("View on Github..."), this};
    connect(_githubButton, &QPushButton::clicked, this, [&]() {
        QDesktopServices::openUrl(QUrl{Utils::GITHUB_LINK});
    });

    _windowLayout->addWidget(_iconLabel, 0, 0, 1, -1, Qt::AlignCenter);
    _windowLayout->addWidget(_titleLabel, 1, 0, 1, -1, Qt::AlignCenter);
    _windowLayout->addWidget(_versionLabel, 2, 0, 1, -1, Qt::AlignCenter);
    _windowLayout->addWidget(_descriptionLabel, 3, 0, 1, -1, Qt::AlignCenter);
    _windowLayout->addWidget(_websiteButton, 4, 0, 1, -1, Qt::AlignCenter);
    _windowLayout->addWidget(_githubButton, 5, 0, 1, -1, Qt::AlignCenter);
    _windowLayout->addWidget(_messageLabel, 6, 0, 1, -1, Qt::AlignCenter);

    // Set style
    // Set the style to match whether the user started dark mode
    setStyle(Utils::isDarkMode());
}
void AboutWindow::translateUI()
{
    // Set property so styling automatically changes
    setProperty("isHan", Settings::isCurrentLocaleHan());

    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (const auto & button, buttons) {
        button->setProperty("isHan", Settings::isCurrentLocaleHan());
        button->style()->unpolish(button);
        button->style()->polish(button);
    }

    _titleLabel->setText(
        QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::PRODUCT_NAME));
    _versionLabel->setText(tr("Build %1").arg(Utils::CURRENT_VERSION));
    _descriptionLabel->setText(
        QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                    Strings::PRODUCT_DESCRIPTION));
    _websiteButton->setText(tr("Visit website..."));
    _githubButton->setText(tr("View on Github..."));
    _messageLabel->setText(
        QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                    Strings::CREDITS_TEXT)
            .arg(palette().text().color().name(),
                 Utils::getLicenseFolderPath() + "FLOW_LAYOUT_LICENSE.txt"));
#ifndef Q_OS_MAC
    setWindowTitle(
        tr("About %1")
            .arg(QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::PRODUCT_NAME)));
#endif

    resize(sizeHint());
}

void AboutWindow::setStyle(bool use_dark)
{
    (void) (use_dark);
#ifdef Q_OS_MAC
    setStyleSheet("QPushButton[isHan=\"true\"] { font-size: 12px; height: 16px; }");
#elif defined(Q_OS_WIN)
    setAttribute(Qt::WA_StyledBackground);
    setObjectName("AboutWindow");
    setStyleSheet("QPushButton[isHan=\"true\"] { "
                  "   font-size: 12px; "
                  "   height: 20px; "
                  "} "
                  ""
                  "QWidget#AboutWindow { "
                  "   background-color: palette(base); "
                  "   border-top: 1px solid palette(alternate-base); "
                  "} ");
#endif
    _messageLabel->setText(
        QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::CREDITS_TEXT)
            .arg(palette().text().color().name()));
}

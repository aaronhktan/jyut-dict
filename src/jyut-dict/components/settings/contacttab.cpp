#include "contacttab.h"

#include "logic/utils/utils.h"
#include "logic/settings/settingsutils.h"
#include "logic/strings/strings.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif

#include <QCoreApplication>
#include <QDesktopServices>
#include <QLocale>
#include <QPixmap>
#include <QStyle>
#include <QTimer>
#include <QUrl>

ContactTab::ContactTab(QWidget *parent)
    : QWidget{parent}
{
    setupUI();
    translateUI();
}

void ContactTab::changeEvent(QEvent *event)
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

void ContactTab::setupUI()
{
    _tabLayout = new QGridLayout{this};
    _tabLayout->setAlignment(Qt::AlignTop);

    _box = new QGroupBox{this};
#ifdef Q_OS_LINUX
    _box->setStyleSheet("QGroupBox { margin-top: -20px; }");
#endif

    _boxLayout = new QGridLayout{_box};
    _boxLayout->setAlignment(Qt::AlignTop);

    _iconLabel = new QLabel{_box};
    _iconLabel->setFixedWidth(50);
    _iconLabel->setFixedHeight(50);
    _iconLabel->setAlignment(Qt::AlignTop);

    QPixmap icon = QPixmap{":/images/org_icon.png"};
    icon.setDevicePixelRatio(devicePixelRatio());
    int iconWidth = devicePixelRatio() * _iconLabel->width();
    int iconHeight = devicePixelRatio() * _iconLabel->height();
    _iconLabel->setPixmap(icon.scaled(iconWidth,
                                      iconHeight,
                                      Qt::KeepAspectRatio,
                                      Qt::SmoothTransformation));

    _titleLabel = new QLabel{_box};
    _titleLabel->setStyleSheet("QLabel { font-size: 18px; }");

    _messageLabel = new QLabel{_box};

    _emailButton = new QPushButton{_box};
    connect(_emailButton, &QPushButton::clicked, this, [&]() {
        QDesktopServices::openUrl(QUrl{Utils::AUTHOR_EMAIL});
    });

    _donateButton = new QPushButton{_box};
    connect(_donateButton, &QPushButton::clicked, this, [&]() {
        QDesktopServices::openUrl(QUrl{Utils::DONATE_LINK});
    });

    _githubButton = new QPushButton{_box};
    connect(_githubButton, &QPushButton::clicked, this, [&]() {
        QDesktopServices::openUrl(QUrl{Utils::AUTHOR_GITHUB_LINK});
    });

    _otherSourcesLabel = new QLabel{this};
    _otherSources = new QLabel{this};
    _otherSources->setOpenExternalLinks(true);

    _boxLayout->addWidget(_iconLabel, 2, 0, 2, 1);
#ifdef Q_OS_WIN
    _boxLayout->setColumnMinimumWidth(0, _iconLabel->width() + 5);
#endif
    _boxLayout->addWidget(_titleLabel, 2, 1, 1, -1, Qt::AlignBottom);
    _boxLayout->addWidget(_messageLabel, 3, 1, 1, -1, Qt::AlignTop);
#ifdef Q_OS_MAC
    _boxLayout->setRowMinimumHeight(5, 30);
#elif defined(Q_OS_WIN)
    _boxLayout->setRowMinimumHeight(5, 20);
#else
    _boxLayout->setRowMinimumHeight(5, 50);
#endif
    _boxLayout->addWidget(_emailButton, 5, 1, 1, 1, Qt::AlignBottom);
    _boxLayout->addWidget(_donateButton, 5, 2, 1, 1, Qt::AlignBottom);
    _boxLayout->addWidget(_githubButton, 5, 3, 1, 1, Qt::AlignBottom);

    _tabLayout->addWidget(_box, 1, 0, 1, -1);
    _tabLayout->addWidget(_otherSourcesLabel, 7, 0, 1, -1, Qt::AlignHCenter);
    _tabLayout->addWidget(_otherSources, 8, 0, 1, -1, Qt::AlignHCenter);

    // Set the style to match whether the user started dark mode
    setStyle(Utils::isDarkMode());
}

void ContactTab::translateUI()
{
    // Set property so styling automatically changes
    setProperty("isHan", Settings::isCurrentLocaleHan());

    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (auto button, buttons) {
        button->setProperty("isHan", Settings::isCurrentLocaleHan());
        button->style()->unpolish(button);
        button->style()->polish(button);
    }

    _titleLabel->setText(
        QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                    Strings::CONTACT_TITLE)
            .arg(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                             Strings::PRODUCT_NAME)));
#ifdef Q_OS_WIN
    _messageLabel->setText(
        QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::CONTACT_BODY_NO_EMOJI));
#else
    _messageLabel->setText(
        QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::CONTACT_BODY));
#endif
    _emailButton->setText(tr("Email..."));
    _donateButton->setText(tr("Donate..."));
    _githubButton->setText(tr("View on Github..."));

    _otherSourcesLabel->setText(
        tr("Looking for other Cantonese resources? Try these!"));
    _otherSources->setText(
        QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::OTHER_SOURCES).arg(palette().text().color().name()));
}

void ContactTab::setStyle(bool use_dark)
{
    (void) (use_dark);
#ifdef Q_OS_MAC
    setStyleSheet("QPushButton[isHan=\"true\"] { font-size: "
                  "13px; height: 16px; }");
#elif defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    setAttribute(Qt::WA_StyledBackground);
    setObjectName("ContactTab");
    setStyleSheet("QPushButton[isHan=\"true\"] { "
                  "   font-size: 12px; height: 20px; "
                  "}"
                  ""
                  "QWidget#ContactTab { "
                  "   background-color: palette(base);"
                  "} ");
#endif
    _otherSources->setText(
        QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::OTHER_SOURCES).arg(palette().text().color().name()));
}


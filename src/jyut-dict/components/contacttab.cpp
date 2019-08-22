#include "contacttab.h"

#include "logic/utils/utils.h"

#include <QDesktopServices>
#include <QPixmap>
#include <QUrl>
#include <QTimer>

ContactTab::ContactTab(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void ContactTab::setupUI()
{
    _tabLayout = new QGridLayout{this};
    _tabLayout->setAlignment(Qt::AlignTop);

    _box = new QGroupBox{this};

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
    _titleLabel->setText(tr(Utils::CONTACT_TITLE));

    _messageLabel = new QLabel{_box};
    _messageLabel->setText(tr(Utils::CONTACT_BODY));

    _emailButton = new QPushButton{tr("Email..."), _box};
    connect(_emailButton, &QPushButton::clicked, this, [&]() {
        QDesktopServices::openUrl(QUrl{Utils::AUTHOR_EMAIL});
    });

    _donateButton = new QPushButton{tr("Donate..."), _box};
    connect(_donateButton, &QPushButton::clicked, this, [&]() {
        QDesktopServices::openUrl(QUrl{Utils::DONATE_LINK});
    });

    _githubButton = new QPushButton{tr("View on Github..."), _box};
    connect(_githubButton, &QPushButton::clicked, this, [&]() {
        QDesktopServices::openUrl(QUrl{Utils::AUTHOR_GITHUB_LINK});
    });

    _otherSourcesLabel = new QLabel{
        tr("Looking for other Cantonese resources? Try these!"), this};
    _otherSources = new QLabel{this};
    _otherSources->setText(
        "    " + tr(Utils::OTHER_SOURCES).arg(palette().text().color().name()));

    _boxLayout->addWidget(_iconLabel, 2, 0, 2, 1);
    _boxLayout->addWidget(_titleLabel, 2, 1, 1, -1, Qt::AlignBottom);
    _boxLayout->addWidget(_messageLabel, 3, 1, 1, -1, Qt::AlignTop);
    _boxLayout->setRowMinimumHeight(5, 30);
    _boxLayout->addWidget(_emailButton, 5, 1, 1, 1, Qt::AlignBottom);
    _boxLayout->addWidget(_donateButton, 5, 2, 1, 1, Qt::AlignBottom);
    _boxLayout->addWidget(_githubButton, 5, 3, 1, 1, Qt::AlignBottom);

    _tabLayout->addWidget(_box, 1, 0, 1, -1);
    _tabLayout->addWidget(_otherSourcesLabel, 7, 0, 1, -1, Qt::AlignHCenter);
    _tabLayout->addWidget(_otherSources, 8, 0, 1, -1, Qt::AlignHCenter);
}

void ContactTab::changeEvent(QEvent *event)
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
    QWidget::changeEvent(event);
}

void ContactTab::setStyle(bool use_dark)
{
    _otherSources->setText(
        tr("Try these:")
        + tr(Utils::OTHER_SOURCES).arg(palette().text().color().name()));
}


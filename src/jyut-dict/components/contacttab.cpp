#include "contacttab.h"

#include <QDesktopServices>
#include <QPixmap>
#include <QUrl>

#include <QDebug>
ContactTab::ContactTab(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void ContactTab::setupUI()
{
    _tabLayout = new QGridLayout{this};
    _tabLayout->setAlignment(Qt::AlignTop);

    _iconLabel = new QLabel{this};
    _iconLabel->setFixedWidth(75);
    _iconLabel->setFixedHeight(75);
    _iconLabel->setAlignment(Qt::AlignTop);

    QPixmap icon = QPixmap{":/images/icon.png"};
    icon.setDevicePixelRatio(devicePixelRatio());
    int iconWidth = devicePixelRatio() * _iconLabel->width();
    int iconHeight = devicePixelRatio() * _iconLabel->height();
    _iconLabel->setPixmap(icon.scaled(iconWidth,
                                      iconHeight,
                                      Qt::KeepAspectRatio,
                                      Qt::SmoothTransformation));

    _titleLabel = new QLabel{this};
    _titleLabel->setStyleSheet("QLabel { "
                               "    font-size: 20px; "
                               "} ");
    _titleLabel->setText(tr("Jyut Dictionary"));

    _messageLabel = new QLabel{this};
    _messageLabel->setStyleSheet("QLabel { color: grey }");
    _messageLabel->setText(
        tr("Jyut Dictionary is a free, open-source offline "
           "Cantonese Dictionary.<br>Created and maintained by "
           "Aaron Tan."));
    _messageLabel->setMinimumSize(_messageLabel->sizeHint());
    _messageLabel->setMaximumSize(_messageLabel->sizeHint());

    _emailButton = new QPushButton{tr("Email..."), this};
    connect(_emailButton, &QPushButton::clicked, this, [&]() {
        QDesktopServices::openUrl(QUrl{"mailto: hi@aaronhktan.com"});
    });

    _donateButton = new QPushButton{tr("Donate..."), this};
    connect(_donateButton, &QPushButton::clicked, this, [&]() {
        QDesktopServices::openUrl(QUrl{"https://www.paypal.me/cheeseisdisgusting"});
    });

    _githubButton = new QPushButton{tr("View on Github..."), this};
    connect(_githubButton, &QPushButton::clicked, this, [&]() {
        QDesktopServices::openUrl(QUrl{"https://github.com/aaronhktan/jyut-dict"});
    });

    _tabLayout->addWidget(_iconLabel, 1, 0, 4, 1);
    _tabLayout->addWidget(_titleLabel, 2, 1, 1, -1);
    _tabLayout->addWidget(_messageLabel, 3, 1, 1, -1);
    _tabLayout->addWidget(_emailButton, 5, 1, 1, 1);
    _tabLayout->addWidget(_donateButton, 5, 2, 1, 1);
    _tabLayout->addWidget(_githubButton, 5, 3, 1, 1);
}


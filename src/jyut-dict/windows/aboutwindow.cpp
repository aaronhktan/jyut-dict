#include "aboutwindow.h"

#include "logic/utils/utils.h"

#include <QApplication>
#include <QDesktopServices>
#include <QPixmap>
#include <QString>
#include <QTimer>
#include <QUrl>

AboutWindow::AboutWindow(QWidget *parent)
    : QWidget{parent, Qt::Window}
{
    setupUI();
    resize(sizeHint());
    move(parent->x() + (parent->width() - sizeHint().width()) / 2,
         parent->y() + (parent->height() - sizeHint().height()) / 2);

    setAttribute(Qt::WA_DeleteOnClose);
}

void AboutWindow::setupUI()
{
    _windowLayout = new QGridLayout{this};

    _iconLabel = new QLabel{this};
    _iconLabel->setFixedWidth(100);
    _iconLabel->setFixedHeight(100);
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
    _titleLabel->setStyleSheet("QLabel { font-size: 20px; } ");
    _titleLabel->setText(tr(Utils::PRODUCT_NAME));

    _versionLabel = new QLabel{this};
    _versionLabel->setStyleSheet("QLabel { font-size: 10px; } ");
    _versionLabel->setText(tr("Build %1").arg(Utils::CURRENT_VERSION));

    _descriptionLabel = new QLabel{this};
    _descriptionLabel->setText(tr(Utils::PRODUCT_DESCRIPTION));
    _descriptionLabel->setAlignment(Qt::AlignHCenter);
    _descriptionLabel->setWordWrap(true);

    _messageLabel = new QLabel{this};
    _messageLabel->setAlignment(Qt::AlignHCenter);
    _messageLabel->setStyleSheet("QLabel { font-size: 10px; }");
    _messageLabel->setText(
        tr(Utils::CREDITS_TEXT).arg(palette().text().color().name()));
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

    _githubButton = new QPushButton{tr("View on Github..."), this};
    connect(_githubButton, &QPushButton::clicked, this, [&]() {
        QDesktopServices::openUrl(QUrl{Utils::GITHUB_LINK});
    });

    _windowLayout->addWidget(_iconLabel, 0, 0, 1, -1, Qt::AlignCenter);
    _windowLayout->addWidget(_titleLabel, 1, 0, 1, -1, Qt::AlignCenter);
    _windowLayout->addWidget(_versionLabel, 2, 0, 1, -1, Qt::AlignCenter);
    _windowLayout->addWidget(_githubButton, 4, 0, 1, -1, Qt::AlignCenter);
    _windowLayout->addWidget(_descriptionLabel, 3, 0, 1, -1, Qt::AlignCenter);
    _windowLayout->addWidget(_messageLabel, 5, 0, 1, -1, Qt::AlignCenter);
}

void AboutWindow::changeEvent(QEvent *event)
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

void AboutWindow::setStyle(bool use_dark)
{
    _messageLabel->setText(
        tr(Utils::CREDITS_TEXT).arg(palette().text().color().name()));
}

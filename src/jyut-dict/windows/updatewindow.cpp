#include "updatewindow.h"

#include <QDesktopServices>
#include <QFont>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QSize>

UpdateWindow::UpdateWindow(QWidget *parent,
                           std::string versionNumber,
                           std::string url, std::string description)
    : QWidget(parent, Qt::Window)
{
    _dialogLayout = new QGridLayout{this};
    _dialogLayout->setSpacing(10);

    _url = url;

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    setWindowTitle(tr("Update Available!"));
#endif
    Qt::WindowFlags flags = windowFlags() | Qt::CustomizeWindowHint | Qt::WindowTitleHint;
    flags &= ~(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint | Qt::WindowFullscreenButtonHint);
    setWindowFlags(flags);

    _iconLabel = new QLabel{this};
    _iconLabel->setFixedWidth(75);
    _iconLabel->setFixedHeight(75);
    _iconLabel->setAlignment(Qt::AlignTop);

    QPixmap icon = QPixmap{":/images/icon.png"};
    icon.setDevicePixelRatio(devicePixelRatio());
    int iconWidth = devicePixelRatio() * _iconLabel->width() - 10;
    int iconHeight = devicePixelRatio() * _iconLabel->height() - 10;
    _iconLabel->setPixmap(icon.scaled(iconWidth, iconHeight,
                                      Qt::KeepAspectRatio,
                                      Qt::SmoothTransformation));

    _titleLabel = new QLabel{this};
    _titleLabel->setStyleSheet("QLabel { font-weight: bold }");
    _titleLabel->setText(tr("A new version of %1 is available!").arg(tr(Utils::PRODUCT_NAME)));

    _messageLabel = new QLabel{this};
    _messageLabel->setWordWrap(true);
    _messageLabel->setStyleSheet("QLabel { color: grey; }");
    _messageLabel->setText(tr("%1 version %2 is available — you "
                              "have version %3. "
                              "Click \"Download\" to get the new version.")
                               .arg(tr(Utils::PRODUCT_NAME))
                               .arg(QString{versionNumber.c_str()})
                               .arg(Utils::CURRENT_VERSION));
    _messageLabel->setFixedWidth(375);
    _messageLabel->setAlignment(Qt::AlignTop);

    _descriptionTextEdit = new QTextEdit{this};
    _descriptionTextEdit->setText(description.c_str());
    _descriptionTextEdit->setContentsMargins(10, 10, 0, 10);
    _descriptionTextEdit->setTextInteractionFlags(Qt::NoTextInteraction);
    _descriptionTextEdit->setAlignment(Qt::AlignTop);
    _descriptionTextEdit->hide();

    _spacer = new QWidget{this};
    _spacer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    _noButton = new QPushButton{this};
    _noButton->setText(tr("Cancel"));

    _showMoreButton = new QPushButton{this};
    _showMoreButton->setText(tr("Show Details"));

    _okButton = new QPushButton{this};
    _okButton->setDefault(true);
    _okButton->setText(tr("Download"));

    connect(_noButton, &QPushButton::clicked, this, &UpdateWindow::noAction);
    connect(_showMoreButton, &QPushButton::clicked, this, &UpdateWindow::showDetails);
    connect(_okButton, &QPushButton::clicked, this, &UpdateWindow::OKAction);

    _dialogLayout->addWidget(_iconLabel, 1, 0, 3, 1);
    _dialogLayout->addWidget(_titleLabel, 1, 1, 1, -1);
    _dialogLayout->addWidget(_messageLabel, 2, 1, 2, -1);
    _dialogLayout->addWidget(_spacer, 4, 1, 1, -1);
    _dialogLayout->addWidget(_noButton, 7, 3, 1, 1);
    _dialogLayout->addWidget(_showMoreButton, 7, 1, 1, 1);
    _dialogLayout->addWidget(_okButton, 7, 4, 1, 1);

    setLayout(_dialogLayout);

    move(parent->x() + (parent->width() - sizeHint().width()) / 2,
      parent->y() + (parent->height() - sizeHint().height()) / 2);
}

UpdateWindow::~UpdateWindow()
{

}

void UpdateWindow::showDetails()
{
    _dialogLayout->replaceWidget(_spacer, _descriptionTextEdit);
    _showMoreButton->setText(tr("Hide details"));
    disconnect(_showMoreButton, nullptr, nullptr, nullptr);
    connect(_showMoreButton, &QPushButton::clicked, this, &UpdateWindow::hideDetails);

    QPropertyAnimation *animation = new QPropertyAnimation{this, "size", this};
    animation->setDuration(150);
    animation->setEndValue(QSize{600, 400});
    animation->start();

    _descriptionTextEdit->show();
}

void UpdateWindow::hideDetails()
{
    // Remove the text edit, and use QWidget to take up space so animation
    // doesn't look weird.
    _dialogLayout->replaceWidget(_descriptionTextEdit, _spacer);
    _descriptionTextEdit->hide();

    _showMoreButton->setText(tr("Show details"));
    disconnect(_showMoreButton, nullptr, nullptr, nullptr);
    connect(_showMoreButton, &QPushButton::clicked, this, &UpdateWindow::showDetails);

    QPropertyAnimation *animation = new QPropertyAnimation{this, "size", this};
    animation->setDuration(150);
    animation->setEndValue(minimumSizeHint());
    animation->start();
}

void UpdateWindow::noAction()
{
    close();
}

void UpdateWindow::OKAction()
{
    QDesktopServices::openUrl(QUrl{_url.c_str()});
}

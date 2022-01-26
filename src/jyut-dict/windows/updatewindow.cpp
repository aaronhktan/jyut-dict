#include "updatewindow.h"

#include "logic/strings/strings.h"
#include "logic/settings/settingsutils.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#endif

#include <QCoreApplication>
#include <QDesktopServices>
#include <QFont>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QSize>
#include <QStyle>

UpdateAvailableWindow::UpdateAvailableWindow(QWidget *parent,
                           std::string versionNumber,
                           std::string url, std::string description)
    : QWidget(parent, Qt::Window)
{
    _versionNumber = versionNumber;
    _url = url;
    _description = description;

    setupUI();
    translateUI();

    Qt::WindowFlags flags = windowFlags() | Qt::CustomizeWindowHint | Qt::WindowTitleHint;
    flags &= ~(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint | Qt::WindowFullscreenButtonHint);
    setWindowFlags(flags);

    move(parent->x() + (parent->width() - sizeHint().width()) / 2,
      parent->y() + (parent->height() - sizeHint().height()) / 2);
}

void UpdateAvailableWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QWidget::changeEvent(event);
}

void UpdateAvailableWindow::setupUI()
{
    _dialogLayout = new QGridLayout{this};
    _dialogLayout->setSpacing(10);

    _iconLabel = new QLabel{this};
    _iconLabel->setFixedWidth(75);
    _iconLabel->setFixedHeight(75);
    _iconLabel->setAlignment(Qt::AlignTop);

    QPixmap icon = QPixmap{":/images/icon.png"};
    icon.setDevicePixelRatio(devicePixelRatio());
    int iconWidth = devicePixelRatio() * _iconLabel->width() - 10;
    int iconHeight = devicePixelRatio() * _iconLabel->height() - 10;
    _iconLabel->setPixmap(icon.scaled(iconWidth,
                                      iconHeight,
                                      Qt::KeepAspectRatio,
                                      Qt::SmoothTransformation));

    _titleLabel = new QLabel{this};
    _titleLabel->setStyleSheet("QLabel { font-weight: bold }");

    _messageLabel = new QLabel{this};
    _messageLabel->setWordWrap(true);
    _messageLabel->setStyleSheet("QLabel { color: grey; }");
    _messageLabel->setFixedWidth(375);
    _messageLabel->setAlignment(Qt::AlignTop);

    _descriptionTextEdit = new QTextEdit{this};
    _descriptionTextEdit->setText(_description.c_str());
    _descriptionTextEdit->setContentsMargins(10, 10, 0, 10);
    _descriptionTextEdit->setTextInteractionFlags(Qt::NoTextInteraction);
    _descriptionTextEdit->setAlignment(Qt::AlignTop);
    _descriptionTextEdit->hide();

    _spacer = new QWidget{this};
    _spacer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    _noButton = new QPushButton{this};

    _showMoreButton = new QPushButton{this};

    _okButton = new QPushButton{this};
    _okButton->setDefault(true);

    connect(_noButton, &QPushButton::clicked, this, &UpdateAvailableWindow::noAction);
    connect(_showMoreButton,
            &QPushButton::clicked,
            this,
            &UpdateAvailableWindow::showDetails);
    connect(_okButton, &QPushButton::clicked, this, &UpdateAvailableWindow::OKAction);

    _dialogLayout->addWidget(_iconLabel, 1, 0, 3, 1);
    _dialogLayout->addWidget(_titleLabel, 1, 1, 1, -1);
    _dialogLayout->addWidget(_messageLabel, 2, 1, 2, -1);
    _dialogLayout->addWidget(_spacer, 4, 1, 1, -1);
    _dialogLayout->addWidget(_noButton, 7, 3, 1, 1);
    _dialogLayout->addWidget(_showMoreButton, 7, 1, 1, 1);
    _dialogLayout->addWidget(_okButton, 7, 4, 1, 1);

    setLayout(_dialogLayout);

#ifdef Q_OS_MAC
    // Set the style to match whether the user started dark mode
    setStyle(Utils::isDarkMode());
#else
    setStyle(false);
#endif
}

void UpdateAvailableWindow::translateUI()
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
        tr("A new version of %1 is available!")
            .arg(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                             Strings::PRODUCT_NAME)));
    _messageLabel->setText(
        tr("%1 version %2 is available — you "
           "have version %3. "
           "Click \"Download\" to get the new version.")
            .arg(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                             Strings::PRODUCT_NAME))
            .arg(QString{_versionNumber.c_str()})
            .arg(Utils::CURRENT_VERSION));
    _noButton->setText(tr("Cancel"));
    _showMoreButton->setText(tr("Show Details"));
    _okButton->setText(tr("Download"));

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    setWindowTitle(tr("Update Available!"));
#endif

    resize(sizeHint());
}

void UpdateAvailableWindow::setStyle(bool use_dark)
{
    (void) (use_dark);
#ifdef Q_OS_MAC
    setStyleSheet("QPushButton[isHan=\"true\"] { font-size: 12px; height: 16px; }");
#elif defined(Q_OS_WIN)
    setStyleSheet("QPushButton[isHan=\"true\"] { font-size: 12px; height: 20px; }");
#endif
}

void UpdateAvailableWindow::showDetails()
{
    _dialogLayout->replaceWidget(_spacer, _descriptionTextEdit);
    _showMoreButton->setText(tr("Hide details"));
    disconnect(_showMoreButton, nullptr, nullptr, nullptr);
    connect(_showMoreButton, &QPushButton::clicked, this, &UpdateAvailableWindow::hideDetails);

    QPropertyAnimation *animation = new QPropertyAnimation{this, "size", this};
    animation->setDuration(150);
    animation->setEndValue(QSize{600, 400});
    animation->start();

    _descriptionTextEdit->show();
}

void UpdateAvailableWindow::hideDetails()
{
    // Remove the text edit, and use QWidget to take up space so animation
    // doesn't look weird.
    _dialogLayout->replaceWidget(_descriptionTextEdit, _spacer);
    _descriptionTextEdit->hide();

    _showMoreButton->setText(tr("Show details"));
    disconnect(_showMoreButton, nullptr, nullptr, nullptr);
    connect(_showMoreButton, &QPushButton::clicked, this, &UpdateAvailableWindow::showDetails);

    QPropertyAnimation *animation = new QPropertyAnimation{this, "size", this};
    animation->setDuration(150);
    animation->setEndValue(minimumSizeHint());
    animation->start();
}

void UpdateAvailableWindow::noAction()
{
    close();
}

void UpdateAvailableWindow::OKAction()
{
    QDesktopServices::openUrl(QUrl{_url.c_str()});
}

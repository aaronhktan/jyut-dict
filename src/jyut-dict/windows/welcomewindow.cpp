#include "welcomewindow.h"

#include "logic/settings/settingsutils.h"
#include "logic/strings/strings.h"
#include "logic/utils/utils.h"
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

WelcomeWindow::WelcomeWindow(QWidget *parent)
    : QWidget(parent, Qt::Window)
{
    _settings = Settings::getSettings();

    setupUI();
    translateUI();

    Qt::WindowFlags flags = windowFlags() | Qt::CustomizeWindowHint | Qt::WindowTitleHint;
    flags &= ~(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint | Qt::WindowFullscreenButtonHint);
    setWindowFlags(flags);
    setWindowModality(Qt::ApplicationModal);

    move(parent->x() + (parent->width() - sizeHint().width()) / 2,
      parent->y() + (parent->height() - sizeHint().height()) / 2);

    layout()->setSizeConstraint(QLayout::SetFixedSize);
}

void WelcomeWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QWidget::changeEvent(event);
}

void WelcomeWindow::setupUI()
{
    _dialogLayout = new QGridLayout{this};
    _dialogLayout->setSpacing(5);
    _dialogLayout->setContentsMargins(22, 11, 22, 22);

    _iconLabel = new QLabel{this};
    _iconLabel->setFixedHeight(75);
    _iconLabel->setAlignment(Qt::AlignHCenter);

    QPixmap icon = QPixmap{":/images/icon.png"};
    icon.setDevicePixelRatio(devicePixelRatio());
    int iconWidth = devicePixelRatio() * _iconLabel->height();
    int iconHeight = devicePixelRatio() * _iconLabel->height();
    _iconLabel->setPixmap(icon.scaled(iconWidth,
                                      iconHeight,
                                      Qt::KeepAspectRatio,
                                      Qt::SmoothTransformation));
    _iconLabel->setStyleSheet("QLabel { padding: 0px; margin-top: 0px; }");

    _titleLabel = new QLabel{this};
    _titleLabel->setStyleSheet(
        "QLabel { font-weight: bold; font-size: 16px; margin-bottom: 11px; }");
    _titleLabel->setAlignment(Qt::AlignCenter);

    _messageLabel = new QLabel{this};
    _messageLabel->setWordWrap(true);
    _messageLabel->setFixedWidth(350);
    _messageLabel->setAlignment(Qt::AlignCenter);

    _leftSpacer = new QWidget{this};
    _leftSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    _rightSpacer = new QWidget{this};
    _rightSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    _buttonSpacer = new QWidget{this};
    _buttonSpacer->setFixedHeight(1);
    _buttonSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    _noButton = new QPushButton{this};
    _noButton->setFixedWidth(80);

    _okButton = new QPushButton{this};
    _okButton->setDefault(true);
    _okButton->setFixedWidth(80);

    connect(_noButton, &QPushButton::clicked, this, &WelcomeWindow::noAction);
    connect(_okButton, &QPushButton::clicked, this, &WelcomeWindow::OKAction);

    _dialogLayout->addWidget(_leftSpacer, 0, 0, 1, 2);
    _dialogLayout->addWidget(_iconLabel, 0, 2, 1, 2);
    _dialogLayout->addWidget(_rightSpacer, 0, 4, 1, 2);
    _dialogLayout->addWidget(_titleLabel, 1, 0, 1, -1);
    _dialogLayout->addWidget(_messageLabel, 2, 0, 1, -1);
    _dialogLayout->addWidget(_buttonSpacer, 3, 0, 1, -1);
    _dialogLayout->addWidget(_noButton, 4, 2, 1, 1);
    _dialogLayout->addWidget(_okButton, 4, 3, 1, 1);

    setLayout(_dialogLayout);

#ifdef Q_OS_MAC
    // Set the style to match whether the user started dark mode
    setStyle(Utils::isDarkMode());
#else
    setStyle(false);
#endif
}

void WelcomeWindow::translateUI()
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
        tr("Welcome to %1 %2!")
            .arg(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                             Strings::PRODUCT_NAME))
            .arg(Utils::CURRENT_VERSION));
    _messageLabel->setText(
        tr("New Feature: Easier Search In Cantonese")
        + QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                      Strings::FUZZY_JYUTPING_EXPLAINER)
        + tr("Would you like to enable this feature?"));
    _noButton->setText(tr("No"));
    _okButton->setText(tr("OK"));

    resize(sizeHint());
}

void WelcomeWindow::setStyle(bool use_dark)
{
    (void) (use_dark);
#ifdef Q_OS_MAC
    setStyleSheet("QPushButton[isHan=\"true\"] { font-size: 12px; height: "
                  "16px; }");
#elif defined(Q_OS_WIN)
    setStyleSheet("QPushButton[isHan=\"true\"] { font-size: 12px; height: "
                  "20px; }");
#endif
}

void WelcomeWindow::noAction(void)
{
    _settings->setValue("Search/fuzzyJyutping", false);
    emit welcomeCompleted();
    close();
}

void WelcomeWindow::OKAction(void)
{
    _settings->setValue("Search/fuzzyJyutping", true);
    emit welcomeCompleted();
    close();
}

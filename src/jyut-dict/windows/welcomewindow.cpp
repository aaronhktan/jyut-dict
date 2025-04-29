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
    _titleLabel->setStyleSheet("QLabel { font-weight: bold; font-size: 16px}");

    _messageLabel = new QLabel{this};
    _messageLabel->setWordWrap(true);
    _messageLabel->setFixedWidth(375);
    _messageLabel->setAlignment(Qt::AlignTop);

    _descriptionTextEdit = new QTextEdit{this};
    // _descriptionTextEdit->setText(_description.c_str());
    _descriptionTextEdit->setContentsMargins(10, 10, 0, 10);
    _descriptionTextEdit->setTextInteractionFlags(Qt::NoTextInteraction);
    _descriptionTextEdit->setAlignment(Qt::AlignTop);
    _descriptionTextEdit->hide();

    _spacer = new QWidget{this};
    _spacer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    _noButton = new QPushButton{this};

    _okButton = new QPushButton{this};
    _okButton->setDefault(true);

    connect(_noButton, &QPushButton::clicked, this, &WelcomeWindow::noAction);
    connect(_okButton, &QPushButton::clicked, this, &WelcomeWindow::OKAction);

    _dialogLayout->addWidget(_iconLabel, 1, 0, 3, 1);
    _dialogLayout->addWidget(_titleLabel, 1, 1, 1, -1);
    _dialogLayout->addWidget(_messageLabel, 2, 1, 2, -1);
    _dialogLayout->addWidget(_spacer, 4, 1, 1, -1);
    _dialogLayout->addWidget(_noButton, 7, 3, 1, 1);
    _dialogLayout->addWidget(_okButton, 7, 4, 1, 1);

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
    setStyleSheet("QPushButton[isHan=\"true\"] { font-size: 12px; height: 16px; }");
#elif defined(Q_OS_WIN)
    setStyleSheet("QPushButton[isHan=\"true\"] { font-size: 12px; height: 20px; }");
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

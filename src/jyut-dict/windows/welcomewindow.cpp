#include "welcomewindow.h"

#include "logic/settings/settings.h"
#include "logic/settings/settingsutils.h"
#include "logic/strings/strings.h"
#include "logic/utils/utils.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined(Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
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

    Qt::WindowFlags flags = windowFlags() | Qt::CustomizeWindowHint | Qt::WindowTitleHint;
    flags &= ~(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint | Qt::WindowFullscreenButtonHint);
    setWindowFlags(flags);
    setWindowModality(Qt::ApplicationModal);

    setupUI();
    translateUI();
}

void WelcomeWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QWidget::changeEvent(event);
}

void WelcomeWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        noAction();
    } else if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        OKAction();
    }
}

void WelcomeWindow::setupUI()
{
#ifdef Q_OS_WIN
    _innerWidget = new QWidget{this};
#endif

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

    _titleLabel = new QLabel{this};
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

#ifdef Q_OS_WIN
    _innerWidget->setLayout(_dialogLayout);
    _outerWidgetLayout = new QGridLayout{this};
    _outerWidgetLayout->setContentsMargins(0, 0, 0, 0);
    _outerWidgetLayout->addWidget(_innerWidget);
#else
    setLayout(_dialogLayout);
#endif

    setStyle(Utils::isDarkMode());

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    setWindowTitle(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                               Strings::PRODUCT_NAME));
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
                                             Strings::PRODUCT_NAME),
                 Utils::CURRENT_VERSION));
    _messageLabel->setText(
        tr("New Feature: Easier Search In Cantonese")
        + QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                      Strings::FUZZY_JYUTPING_EXPLAINER)
        + tr("Would you like to enable this feature?"));
    _noButton->setText(tr("No"));
    _okButton->setText(tr("OK"));

    resize(sizeHint());

#ifndef Q_OS_LINUX
    layout()->setSizeConstraint(QLayout::SetFixedSize);
#endif
}

void WelcomeWindow::setStyle(bool use_dark)
{
    (void) (use_dark);

    int interfaceSize = static_cast<int>(
        _settings
            ->value("Interface/size",
                    QVariant::fromValue(Settings::InterfaceSize::NORMAL))
            .value<Settings::InterfaceSize>());
    int headerFontSize = Settings::h6FontSize.at(
        static_cast<unsigned long>(interfaceSize - 1));
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

#ifdef Q_OS_MAC
    _messageLabel->setStyleSheet("QLabel { margin-bottom: 6px; }");
#elif defined(Q_OS_WIN)
    setStyleSheet("QPushButton[isHan=\"true\"] { font-size: 12px; height: "
                  "20px; }");
#elif defined(Q_OS_LINUX)
    setStyleSheet("QPushButton { margin-left: 5px; margin-right: 5px; }");
#endif

    _iconLabel->setStyleSheet("QLabel { padding: 0px; margin-top: 0px; }");
    _titleLabel->setStyleSheet(QString{
        "QLabel { font-weight: bold; font-size: %1px; margin-bottom: 11px; }"}
                                   .arg(headerFontSize));

#ifdef Q_OS_WIN
    QFont font;
    if (Settings::isCurrentLocaleTraditionalHan()) {
        font = QFont{"Microsoft Jhenghei", 10};
    } else if (Settings::isCurrentLocaleSimplifiedHan()) {
        font = QFont{"Microsoft YaHei", 10};
    } else if (Settings::isCurrentLocaleHan()) {
        font = QFont{"Microsoft YaHei", 10};
    } else {
        font = QFont{"Microsoft YaHei", 10};
    }
    font.setStyleHint(QFont::System, QFont::PreferAntialias);
    _messageLabel->setFont(font);

    _innerWidget->setAttribute(Qt::WA_StyledBackground);
    _innerWidget->setObjectName("innerWidget");
    _innerWidget->setStyleSheet("QWidget#innerWidget {"
                                "   background-color: palette(base);"
                                "} ");
#endif

    resize(sizeHint());

#ifndef Q_OS_LINUX
    layout()->setSizeConstraint(QLayout::SetFixedSize);
#endif
}

void WelcomeWindow::noAction(void)
{
    if (_fuzzyJyutpingDone) {
        _settings->setValue("Search/fuzzyPinyin", false);
        emit welcomeCompleted();
        close();
    } else {
        _settings->setValue("Search/fuzzyJyutping", false);
        _messageLabel->setText(
            tr("New Feature: Easier Search In Mandarin")
            + QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                          Strings::FUZZY_PINYIN_EXPLAINER)
            + tr("Would you like to enable this feature?"));
        _fuzzyJyutpingDone = true;
    }
}

void WelcomeWindow::OKAction(void)
{
    if (_fuzzyJyutpingDone) {
        _settings->setValue("Search/fuzzyPinyin", true);
        emit welcomeCompleted();
        close();
    } else {
        _settings->setValue("Search/fuzzyJyutping", true);
        _messageLabel->setText(
            tr("New Feature: Easier Search In Mandarin")
            + QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                          Strings::FUZZY_PINYIN_EXPLAINER)
            + tr("Would you like to enable this feature?"));
        _fuzzyJyutpingDone = true;
    }
}

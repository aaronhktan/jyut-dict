#include "handwritingwindow.h"

#include "logic/settings/settings.h"
#include "logic/settings/settingsutils.h"
#include "logic/strings/strings.h"
#include "logic/utils/utils.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#endif

#include <QCoreApplication>
#include <QDesktopServices>
#include <QFont>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QSize>
#include <QStyle>

#include <iostream>

HandwritingWindow::HandwritingWindow(QWidget *parent)
    : QWidget{parent, Qt::Window}
{
    _settings = Settings::getSettings();
    _handwritingWrapper = std::make_unique<HandwritingWrapper>(
        Handwriting::Script::TRADITIONAL);

    connect(_handwritingWrapper.get(),
            &HandwritingWrapper::recognizedResults,
            this,
            [&](std::vector<std::string> results) {
                for (const auto &r : results) {
                    std::cout << r << std::endl;
                }
            });

    Qt::WindowFlags flags = windowFlags() | Qt::CustomizeWindowHint
                            | Qt::WindowTitleHint;
    flags &= ~(Qt::WindowMinMaxButtonsHint | Qt::WindowFullscreenButtonHint);
    setWindowFlags(flags);
    setWindowModality(Qt::ApplicationModal);

    setupUI();
    translateUI();
}

void HandwritingWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QWidget::changeEvent(event);
}

void HandwritingWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        // noAction();
    } else if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        // OKAction();
    }
}

void HandwritingWindow::setupUI()
{
#ifdef Q_OS_WIN
    _innerWidget = new QWidget{this};
#endif

    _layout = new QGridLayout{this};

    _panel = new HandwritingPanel{this};
    connect(_panel,
            &HandwritingPanel::pixmapDimensions,
            _handwritingWrapper.get(),
            &HandwritingWrapper::setDimensions);
    connect(_panel,
            &HandwritingPanel::strokeStart,
            _handwritingWrapper.get(),
            &HandwritingWrapper::startStroke);
    connect(_panel,
            &HandwritingPanel::strokeUpdate,
            _handwritingWrapper.get(),
            &HandwritingWrapper::updateStroke);
    connect(_panel,
            &HandwritingPanel::strokeComplete,
            _handwritingWrapper.get(),
            &HandwritingWrapper::completeStroke);

#ifdef Q_OS_WIN
    _innerWidget->setLayout(_dialogLayout);
    _outerWidgetLayout = new QGridLayout{this};
    _outerWidgetLayout->setContentsMargins(0, 0, 0, 0);
    _outerWidgetLayout->addWidget(_innerWidget);
#else
    setLayout(_layout);
#endif

    _layout->addWidget(_panel, 0, 0, -1, 1);

#ifdef Q_OS_MAC
    // Set the style to match whether the user started dark mode
    setStyle(Utils::isDarkMode());
#else
    setStyle(false);
#endif

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    setWindowTitle(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                               Strings::PRODUCT_NAME));
#endif
}

void HandwritingWindow::translateUI()
{
    // Set property so styling automatically changes
    setProperty("isHan", Settings::isCurrentLocaleHan());

    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (const auto & button, buttons) {
        button->setProperty("isHan", Settings::isCurrentLocaleHan());
        button->style()->unpolish(button);
        button->style()->polish(button);
    }

#ifndef Q_OS_LINUX
    layout()->setSizeConstraint(QLayout::SetFixedSize);
#endif
}

void HandwritingWindow::setStyle(bool use_dark)
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

#ifdef Q_OS_WIN
    setStyleSheet("QPushButton[isHan=\"true\"] { font-size: 12px; height: "
                  "20px; }");
#elif defined(Q_OS_LINUX)
    setStyleSheet("QPushButton { margin-left: 5px; margin-right: 5px; }");
#endif

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

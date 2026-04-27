#include "sourceupdateresultwindow.h"

#include "logic/settings/settings.h"
#include "logic/settings/settingsutils.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined(Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif

#include <QCoreApplication>
#include <QDesktopServices>
#include <QEvent>
#include <QFont>
#include <QGridLayout>
#include <QLabel>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QSettings>
#include <QSize>
#include <QStyle>
#include <QTextEdit>

SourceUpdateResultWindow::SourceUpdateResultWindow(UpdateResult result,
                                                   std::string description,
                                                   QWidget *parent)
    : QWidget{parent, Qt::Dialog}
    , _result{result}
    , _description{description}
    , _settings{Settings::getSettings()}
{
    setObjectName("SourceUpdateResultWindow");
    setupUI();
    translateUI();

    Qt::WindowFlags flags = windowFlags() | Qt::CustomizeWindowHint
                            | Qt::WindowTitleHint;
    flags &= ~(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint
               | Qt::WindowFullscreenButtonHint);
    setWindowFlags(flags);

    move(parent->x() + (parent->width() - sizeHint().width()) / 2,
         parent->y() + (parent->height() - sizeHint().height()) / 2);

    setAttribute(Qt::WA_DeleteOnClose);
}

void SourceUpdateResultWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QWidget::changeEvent(event);
}

void SourceUpdateResultWindow::setupUI()
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

    _showMoreButton = new QPushButton{this};

    _okButton = new QPushButton{this};
    _okButton->setDefault(true);

    connect(_showMoreButton,
            &QPushButton::clicked,
            this,
            &SourceUpdateResultWindow::showDetails);
    connect(_okButton,
            &QPushButton::clicked,
            this,
            &SourceUpdateResultWindow::OKAction);

    _dialogLayout->addWidget(_iconLabel, 1, 0, 3, 1);
    _dialogLayout->addWidget(_titleLabel, 1, 1, 1, -1);
    _dialogLayout->addWidget(_messageLabel, 2, 1, 2, -1);
    _dialogLayout->addWidget(_spacer, 4, 1, 1, -1);
    _dialogLayout->addWidget(_showMoreButton, 7, 1, 1, 1);
    _dialogLayout->addWidget(_okButton, 7, 4, 1, 1);

    setLayout(_dialogLayout);

    setStyle(Utils::isDarkMode());
}

void SourceUpdateResultWindow::translateUI()
{
    // Set property so styling automatically changes
    setProperty("isHan", Settings::isCurrentLocaleHan());

    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (const auto &button, buttons) {
        button->setProperty("isHan", Settings::isCurrentLocaleHan());
        button->style()->unpolish(button);
        button->style()->polish(button);
    }

    QString titleLabelText;
    switch (_result) {
    case UpdateResult::kAllSucceeded: {
        titleLabelText = tr("All dictionary updates succeeded!");
        break;
    }
    case UpdateResult::kSomeSucceeded: {
        titleLabelText = tr("Some dictionary updates succeeded!");
        break;
    }
    case UpdateResult::kNoneSucceeded: {
        titleLabelText = tr("Dictionary updates failed!");
        break;
    }
    }

    _titleLabel->setText(titleLabelText);
    _messageLabel->setText(
        tr("Click 'Show Details' for more information. Please report errors to "
           "the developer of the application."));
    _showMoreButton->setText(tr("Show Details"));
    _okButton->setText(tr("OK"));

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    setWindowTitle(tr("Dictionary Update Results"));
#endif

    resize(sizeHint());
}

void SourceUpdateResultWindow::setStyle(bool use_dark)
{
    (void) (use_dark);
    int interfaceSize = static_cast<int>(
        _settings
            ->value("Interface/size",
                    QVariant::fromValue(Settings::InterfaceSize::NORMAL))
            .value<Settings::InterfaceSize>());
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
                  "} "
                  "QWidget#SourceUpdateResultWindow { "
                  "   background-color: palette(base);"
                  "} "};
    setAttribute(Qt::WA_StyledBackground);
#endif
    setStyleSheet(style.arg(std::to_string(bodyFontSizeHan).c_str(),
                            std::to_string(bodyFontSize).c_str()));
}

void SourceUpdateResultWindow::showDetails()
{
    _dialogLayout->replaceWidget(_spacer, _descriptionTextEdit);
    _showMoreButton->setText(tr("Hide details"));
    disconnect(_showMoreButton, nullptr, nullptr, nullptr);
    connect(_showMoreButton,
            &QPushButton::clicked,
            this,
            &SourceUpdateResultWindow::hideDetails);

    QPropertyAnimation *animation = new QPropertyAnimation{this, "size", this};
    animation->setDuration(150);
    animation->setEndValue(QSize{600, 400});
    animation->start();

    _descriptionTextEdit->show();
}

void SourceUpdateResultWindow::hideDetails()
{
    // Remove the text edit, and use QWidget to take up space so animation
    // doesn't look weird.
    _dialogLayout->replaceWidget(_descriptionTextEdit, _spacer);
    _descriptionTextEdit->hide();

    _showMoreButton->setText(tr("Show details"));
    disconnect(_showMoreButton, nullptr, nullptr, nullptr);
    connect(_showMoreButton,
            &QPushButton::clicked,
            this,
            &SourceUpdateResultWindow::showDetails);

    QPropertyAnimation *animation = new QPropertyAnimation{this, "size", this};
    animation->setDuration(150);
    animation->setEndValue(minimumSizeHint());
    animation->start();
}

void SourceUpdateResultWindow::OKAction()
{
    close();
}

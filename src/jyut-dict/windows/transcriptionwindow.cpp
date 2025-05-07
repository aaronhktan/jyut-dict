#include "transcriptionwindow.h"

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

#include <algorithm>

namespace {
constexpr auto GRAPHICS_VIEW_WIDTH = 200;
constexpr auto GRAPHICS_VIEW_HEIGHT = 100;
constexpr auto MIN_CIRCLE_RADIUS = 25;
constexpr auto MAX_CIRCLE_RADIUS = 50;
} // namespace

TranscriptionWindow::TranscriptionWindow(QWidget *parent)
    : QWidget(parent, Qt::Window)
{
    _settings = Settings::getSettings();
    std::string locale{"zh_HK"};
    _wrapper = std::make_unique<TranscriberWrapper>(locale);
    _wrapper->subscribe(static_cast<IInputVolumeSubscriber *>(this));
    _wrapper->subscribe(static_cast<ITranscriptionResultSubscriber *>(this));

#ifdef Q_OS_MAC
    setWindowFlags(Qt::Sheet);
#else
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
#endif
    setWindowModality(Qt::ApplicationModal);

    setupUI();
    translateUI();

    connect(this,
            &TranscriptionWindow::newRadius,
            this,
            &TranscriptionWindow::startAnimation);

    _wrapper->startRecognition();
}

TranscriptionWindow::~TranscriptionWindow()
{
    _wrapper->unsubscribe(static_cast<IInputVolumeSubscriber *>(this));
    _wrapper->unsubscribe(static_cast<ITranscriptionResultSubscriber *>(this));
}

void TranscriptionWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QWidget::changeEvent(event);
}

void TranscriptionWindow::volumeResult(
    std::variant<std::system_error, float> result)
{
    if (std::system_error *e = std::get_if<std::system_error>(&result)) {
        emit newRadius(MIN_CIRCLE_RADIUS);
    } else {
        float percentage = std::get<float>(result);
        int radius = std::min(percentage
                                      * (MAX_CIRCLE_RADIUS - MIN_CIRCLE_RADIUS)
                                  + MIN_CIRCLE_RADIUS,
                              float(MAX_CIRCLE_RADIUS));
        if (radius % 2) {
            ++radius;
        }
        emit newRadius(radius);
    }
}

void TranscriptionWindow::transcriptionResult(
    std::variant<std::system_error, std::string> result)
{
    if (std::string *t = std::get_if<std::string>(&result)) {
        _titleLabel->setText(t->c_str());
        emit transcription(_titleLabel->text());
    } else {
        if (std::get<std::system_error>(result).code().value() == ETIMEDOUT) {
            doneAction();
        } else {
            stopTranscription();
            qDebug() << std::get<std::system_error>(result).code().value()
                     << std::get<std::system_error>(result).what();
        }
    }
}

void TranscriptionWindow::setupUI()
{
#ifdef Q_OS_WIN
    _innerWidget = new QWidget{this};
#endif

    _dialogLayout = new QGridLayout{this};
    _dialogLayout->setSpacing(5);
    _dialogLayout->setContentsMargins(22, 11, 22, 22);

    QPixmap icon = QPixmap{":/images/mic_action_inverted.png"};
    icon.setDevicePixelRatio(devicePixelRatio());
    _icon = new QGraphicsPixmapItem{icon};
    _icon->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);

    _titleLabel = new QLabel{this};
    _titleLabel->setAlignment(Qt::AlignCenter);

    _graphicsScene = new QGraphicsScene{this};
    _graphicsScene->setSceneRect(0,
                                 0,
                                 GRAPHICS_VIEW_WIDTH,
                                 GRAPHICS_VIEW_HEIGHT);

    _graphicsView = new QGraphicsView{this};
    _graphicsView->setFixedSize(GRAPHICS_VIEW_WIDTH, GRAPHICS_VIEW_HEIGHT);
    _graphicsView->setRenderHints(QPainter::Antialiasing);

    _ellipse
        = new AnimateableEllipse(GRAPHICS_VIEW_WIDTH / 2 - MIN_CIRCLE_RADIUS,
                                 GRAPHICS_VIEW_HEIGHT / 2 - MIN_CIRCLE_RADIUS,
                                 MIN_CIRCLE_RADIUS * 2,
                                 MIN_CIRCLE_RADIUS * 2);
    _ellipse->setParent(_graphicsScene);
    _ellipse->setBrush(palette().brush(QPalette::Base));
    _ellipse->setPen(QPen{Qt::transparent});
    _graphicsScene->addItem(_ellipse);

    _icon->setOffset(GRAPHICS_VIEW_WIDTH / 2 - _icon->boundingRect().width() / 2,
                     GRAPHICS_VIEW_HEIGHT / 2
                         - _icon->boundingRect().height() / 2);
    _graphicsScene->addItem(_icon);

    _graphicsView->setScene(_graphicsScene);
    _graphicsView->setBackgroundBrush(palette().brush(QPalette::Window));
    _graphicsView->setFrameStyle(0);

    _doneButton = new QPushButton{this};
    _doneButton->setDefault(true);
    _doneButton->setFixedWidth(80);

    connect(_doneButton,
            &QPushButton::clicked,
            this,
            &TranscriptionWindow::doneAction);

    _dialogLayout->addWidget(_titleLabel, 0, 0, 1, -1);
    _dialogLayout->addWidget(_graphicsView, 1, 0, 1, -1);
    _dialogLayout->addWidget(_doneButton, 4, 3, 1, 1);

#ifdef Q_OS_WIN
    _innerWidget->setLayout(_dialogLayout);
    _outerWidgetLayout = new QGridLayout{this};
    _outerWidgetLayout->setContentsMargins(0, 0, 0, 0);
    _outerWidgetLayout->addWidget(_innerWidget);
#else
    setLayout(_dialogLayout);
#endif

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

void TranscriptionWindow::translateUI()
{
    // Set property so styling automatically changes
    setProperty("isHan", Settings::isCurrentLocaleHan());

    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (const auto & button, buttons) {
        button->setProperty("isHan", Settings::isCurrentLocaleHan());
        button->style()->unpolish(button);
        button->style()->polish(button);
    }

    _titleLabel->setText(tr("Listening..."));
    _doneButton->setText(tr("Done"));

    resize(sizeHint());

#ifndef Q_OS_LINUX
    layout()->setSizeConstraint(QLayout::SetFixedSize);
#endif
}

void TranscriptionWindow::setStyle(bool use_dark)
{
#ifdef Q_OS_MAC
    setStyleSheet("QPushButton[isHan=\"true\"] { font-size: 12px; height: "
                  "16px; }");
#elif defined(Q_OS_WIN)
    setStyleSheet("QPushButton[isHan=\"true\"] { font-size: 12px; height: "
                  "20px; }");
#elif defined(Q_OS_LINUX)
    setStyleSheet("QPushButton { margin-left: 5px; margin-right: 5px; }");
#endif

    _titleLabel->setStyleSheet("QLabel { font-weight: bold; font-size: 16px; "
                               "margin-top: 11px; margin-bottom: 11px; }");

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

    if (use_dark) {
        QPixmap icon = QPixmap{":/images/mic_action_inverted.png"};
        icon.setDevicePixelRatio(devicePixelRatio());
        _icon->setPixmap(icon);
    } else {
        QPixmap icon = QPixmap{":/images/mic_action.png"};
        icon.setDevicePixelRatio(devicePixelRatio());
        _icon->setPixmap(icon);
    }

    resize(sizeHint());

#ifndef Q_OS_LINUX
    layout()->setSizeConstraint(QLayout::SetFixedSize);
#endif
}

void TranscriptionWindow::startTranscription(void)
{
    _wrapper->startRecognition();
}

void TranscriptionWindow::stopTranscription(void)
{
    _wrapper->stopRecognition();
}

void TranscriptionWindow::startAnimation(float radius)
{
    _anim = new QPropertyAnimation(_ellipse, "rect", this);
    _anim->setDuration(50);
    _anim->setStartValue(
        QRect(GRAPHICS_VIEW_WIDTH / 2 - _ellipse->rect().width() / 2,
              GRAPHICS_VIEW_HEIGHT / 2 - _ellipse->rect().height() / 2,
              _ellipse->rect().width(),
              _ellipse->rect().height()));
    _anim->setEndValue(QRect(GRAPHICS_VIEW_WIDTH / 2 - radius,
                             GRAPHICS_VIEW_HEIGHT / 2 - radius,
                             radius * 2,
                             radius * 2));
    _anim->start();
}

void TranscriptionWindow::doneAction(void)
{
    if (_titleLabel->text() != tr("Listening...")) {
        emit transcription(_titleLabel->text());
    }
    stopTranscription();
    close();
}

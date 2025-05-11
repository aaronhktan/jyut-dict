#include "transcriptionwindow.h"

#include "logic/settings/settings.h"
#include "logic/settings/settingsutils.h"
#include "logic/strings/strings.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QCoreApplication>
#include <QDesktopServices>
#include <QFont>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QSize>
#include <QStyle>
#include <QTimer>

#include <algorithm>

namespace {
constexpr auto GRAPHICS_VIEW_WIDTH = 200;
constexpr auto GRAPHICS_VIEW_HEIGHT = 100;
constexpr auto MIN_CIRCLE_RADIUS = 25;
constexpr auto MAX_CIRCLE_RADIUS = 50;

#ifdef Q_OS_WIN
class NoInteractionLineEdit : public QLineEdit
{
    using QLineEdit::QLineEdit;

protected:
    void keyPressEvent(QKeyEvent *event) override
    {
        // Do nothing
    }

    void keyReleaseEvent(QKeyEvent *event) override
    {
        // Do nothing
    }

    void mousePressEvent(QMouseEvent *event) override
    {
        // Do nothing
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {
        // Do nothing
    }
};
#endif
} // namespace

TranscriptionWindow::TranscriptionWindow(QWidget *parent)
    : QWidget(parent, Qt::Window)
{
    setObjectName("TranscriptionWindow");

    _settings = Settings::getSettings();

#ifdef Q_OS_MAC
    setWindowFlags(Qt::Sheet);
#else
    Qt::WindowFlags flags = windowFlags() | Qt::CustomizeWindowHint
                            | Qt::WindowTitleHint;
    flags &= ~(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint
               | Qt::WindowFullscreenButtonHint);
    setWindowFlags(flags);
#endif
    setWindowModality(Qt::ApplicationModal);

    setupUI();
    translateUI();

#ifdef Q_OS_MAC
    // Set the style to match whether the user started dark mode
    setStyle(Utils::isDarkMode());
#else
    setStyle(false);
#endif

    connect(this,
            &TranscriptionWindow::newRadius,
            this,
            &TranscriptionWindow::startAnimation);
    connect(this,
            &TranscriptionWindow::transcriptionError,
            this,
            &TranscriptionWindow::showErrorDialog);

    TranscriptionLanguage lastSelected
        = _settings
              ->value("Transcription/lastSelected",
                      QVariant::fromValue(TranscriptionLanguage::CANTONESE))
              .value<TranscriptionLanguage>();
    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (const auto &button, buttons) {
        if (button->property("data").value<TranscriptionLanguage>()
            == lastSelected) {
            button->click();
            break;
        }
    }
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

void TranscriptionWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape || event->key() == Qt::Key_Enter
        || event->key() == Qt::Key_Return) {
        doneAction();
    }
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
                              static_cast<float>(MAX_CIRCLE_RADIUS));
        if (radius % 2) {
            ++radius;
        }
        emit newRadius(radius);
    }
}

void TranscriptionWindow::transcriptionResult(
    std::variant<std::system_error, std::string> result)
{
#ifdef Q_OS_MAC
    if (std::string *t = std::get_if<std::string>(&result)) {
        QFontMetrics metrics{_titleLabel->font()};
        QString elidedText = metrics.elidedText(QString::fromStdString(*t),
                                                Qt::ElideRight,
                                                _titleLabel->width());
        _titleLabel->setText(elidedText);
        emit transcription(QString::fromStdString(*t));
    } else {
        if (std::get<std::system_error>(result).code().value() == ETIMEDOUT) {
            doneAction();
        } else {
            stopTranscription();
            emit transcriptionError(std::get<std::system_error>(result)
                                        .code()
                                        .value(),
                                    std::get<std::system_error>(result).what());
        }
    }
#elif defined(Q_OS_WIN)
    if (std::system_error *e = std::get_if<std::system_error>(&result)) {
        stopTranscription();
        emit transcriptionError(std::get<std::system_error>(result)
                                    .code()
                                    .value(),
                                std::get<std::system_error>(result).what());
    }
#endif
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

#ifdef Q_OS_MAC
    _titleLabel = new QLabel{this};
    _titleLabel->setAlignment(Qt::AlignCenter);
#endif

#ifdef Q_OS_WIN
    _lineEdit = new NoInteractionLineEdit{this};

    connect(_lineEdit,
            &QLineEdit::textChanged,
            this,
            &TranscriptionWindow::transcription);
#endif

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
    _ellipse->setPen(QPen{Qt::transparent});
    _graphicsScene->addItem(_ellipse);

    _icon->setOffset(GRAPHICS_VIEW_WIDTH / 2 - _icon->boundingRect().width() / 2,
                     GRAPHICS_VIEW_HEIGHT / 2
                         - _icon->boundingRect().height() / 2);
    _graphicsScene->addItem(_icon);

    _graphicsView->setScene(_graphicsScene);
    _graphicsView->setFrameStyle(0);

    _cantoneseButton = new QPushButton{this};
    _cantoneseButton->setCheckable(true);
    _cantoneseButton->setChecked(true);
    _cantoneseButton->setProperty("data",
                                  QVariant::fromValue(
                                      TranscriptionLanguage::CANTONESE));
    _mandarinButton = new QPushButton{this};
    _mandarinButton->setCheckable(true);
    _mandarinButton->setProperty("data",
                                 QVariant::fromValue(
                                     TranscriptionLanguage::MANDARIN));
    _englishButton = new QPushButton{this};
    _englishButton->setCheckable(true);
    _englishButton->setProperty("lastButton", true);
    _englishButton->setProperty("data",
                                QVariant::fromValue(
                                    TranscriptionLanguage::ENGLISH));

    connect(_cantoneseButton,
            &QPushButton::clicked,
            this,
            &TranscriptionWindow::setTranscriptionLang);
    connect(_mandarinButton,
            &QPushButton::clicked,
            this,
            &TranscriptionWindow::setTranscriptionLang);
    connect(_englishButton,
            &QPushButton::clicked,
            this,
            &TranscriptionWindow::setTranscriptionLang);

    QWidget *spacer = new QWidget{this};
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    spacer->setMinimumHeight(6);

    _doneButton = new QPushButton{this};
    _doneButton->setDefault(true);
    _doneButton->setFixedWidth(80);
    _doneButton->setProperty("ignoreStyle", true);

    connect(_doneButton,
            &QPushButton::clicked,
            this,
            &TranscriptionWindow::doneAction);

#ifdef Q_OS_MAC
    _dialogLayout->addWidget(_titleLabel, 0, 0, 1, -1);
#endif
#ifdef Q_OS_WIN
    _dialogLayout->addWidget(_lineEdit, 0, 0, 1, -1);
#endif
    _dialogLayout->addWidget(_graphicsView, 1, 0, 1, -1);
    _dialogLayout->addWidget(_cantoneseButton, 2, 0, 1, -1);
    _dialogLayout->addWidget(_mandarinButton, 3, 0, 1, -1);
    _dialogLayout->addWidget(_englishButton, 4, 0, 1, -1);
    _dialogLayout->addWidget(spacer, 5, 0, 1, -1);
    _dialogLayout->addWidget(_doneButton, 6, 3, 1, 1);

#ifdef Q_OS_WIN
    _innerWidget->setLayout(_dialogLayout);
    _outerWidgetLayout = new QGridLayout{this};
    _outerWidgetLayout->setContentsMargins(0, 0, 0, 0);
    _outerWidgetLayout->addWidget(_innerWidget);
#else
    setLayout(_dialogLayout);
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

#ifdef Q_OS_MAC
    _titleLabel->setText(tr("Listening..."));
#elif defined(Q_OS_WIN)
    _lineEdit->setPlaceholderText(tr("Listening..."));
    _lineEdit->clear();
#endif
    _cantoneseButton->setText(tr("Cantonese"));
    _mandarinButton->setText(tr("Mandarin"));
    _englishButton->setText(tr("English"));
    _doneButton->setText(tr("Done"));

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    setWindowTitle(tr("Dictation"));
#endif

    resize(sizeHint());

#ifndef Q_OS_LINUX
    layout()->setSizeConstraint(QLayout::SetFixedSize);
#endif
}

void TranscriptionWindow::setStyle(bool use_dark)
{
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet("QWidget#TranscriptionWindow { "
                  "   background-color: palette(base); "
                  "} ");

    QColor borderColour = use_dark ? QColor{HEADER_BACKGROUND_COLOUR_DARK_R,
                                            HEADER_BACKGROUND_COLOUR_DARK_G,
                                            HEADER_BACKGROUND_COLOUR_DARK_B}
                                   : QColor{HEADER_BACKGROUND_COLOUR_LIGHT_R,
                                            HEADER_BACKGROUND_COLOUR_LIGHT_G,
                                            HEADER_BACKGROUND_COLOUR_LIGHT_B};
#ifdef Q_OS_LINUX
    borderColour = borderColour.lighter(200);
#endif
    _graphicsView->setBackgroundBrush(palette().brush(QPalette::Window));
    _ellipse->setBrush(borderColour);

    int interfaceSize = static_cast<int>(
        _settings
            ->value("Interface/size",
                    QVariant::fromValue(Settings::InterfaceSize::NORMAL))
            .value<Settings::InterfaceSize>());
    int bodyFontSize = Settings::bodyFontSize.at(
        static_cast<unsigned long>(interfaceSize - 1));
    int borderRadius = static_cast<int>(bodyFontSize * 1);
#ifdef Q_OS_MAC
    int padding = bodyFontSize / 3;
#else
    int padding = bodyFontSize / 6;
#endif
    int paddingHorizontal = bodyFontSize;
    QString styleSheet = "QPushButton { "
                         "   background-color: transparent; "
                         "   border: 2px solid %1; "
                         "   border-radius: %2px; "
                         "   font-size: %3px; "
                         "   padding: %4px; "
                         "   padding-left: %5px; "
                         "   padding-right: %5px; "
                         "} "
                         " "
                         "QPushButton:checked { "
                         "   background-color: %1; "
                         "   border: 2px solid %1; "
                         "   border-radius: %2px; "
                         "   font-size: %3px; "
                         "   padding: %4px; "
                         "   padding-left: %5px; "
                         "   padding-right: %5px; "
                         "} "
                         " "
                         "QPushButton:hover { "
                         "   background-color: %1; "
                         "   border: 2px solid %1; "
                         "   border-radius: %2px; "
                         "   font-size: %3px; "
                         "   padding: %4px; "
                         "   padding-left: %5px; "
                         "   padding-right: %5px; "
                         "} "
                         " ";

    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (const auto &button, buttons) {
        if (button->property("ignoreStyle").value<bool>()) {
            continue;
        }
        button->setStyleSheet(styleSheet.arg(borderColour.name())
                                  .arg(borderRadius)
                                  .arg(bodyFontSize)
                                  .arg(padding)
                                  .arg(paddingHorizontal));
        button->setMinimumHeight(
            std::max(borderRadius * 2,
                     button->fontMetrics().boundingRect(button->text()).height()
                         + 2 * padding + 2 * 2));
    }

#ifdef Q_OS_LINUX
    _doneButton->setStyleSheet(
        "QPushButton { margin-left: 5px; margin-right: 5px; }");
#endif

#ifdef Q_OS_MAC
    _titleLabel->setStyleSheet("QLabel { "
                               "   font-size: 16px; "
                               "   font-weight: bold; "
                               "   margin-top: 11px; "
                               "   margin-bottom: 11px; "
                               "}");
#endif

#ifdef Q_OS_WIN
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

void TranscriptionWindow::setTranscriptionLang(void)
{
    QPushButton *sender = static_cast<QPushButton *>(QObject::sender());
    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (const auto &button, buttons) {
        button->setChecked(button == sender);
    }

    Settings::getSettings()->setValue("Transcription/lastSelected",
                                      sender->property("data"));
    TranscriptionLanguage lang
        = sender->property("data").value<TranscriptionLanguage>();
    emit languageSelected(lang);

    std::string locale;
    switch (lang) {
    case TranscriptionLanguage::CANTONESE: {
        locale = "zh_HK";
        break;
    }
    case TranscriptionLanguage::MANDARIN: {
        if (Settings::getCurrentLocale().territory() == QLocale::Taiwan) {
            locale = "zh_TW";
        } else {
            locale = "zh_CN";
        }
        break;
    }
    case TranscriptionLanguage::ENGLISH: {
        if (Settings::getCurrentLocale().language() == QLocale::French) {
            if (Settings::getCurrentLocale().territory() == QLocale::Canada) {
                locale = "fr_CA";
            } else {
                locale = "fr_FR";
            }
        } else {
            if (Settings::getCurrentLocale().territory()
                == QLocale::UnitedKingdom) {
                locale = "en_UK";
            } else {
                locale = "en_US";
            }
        }
        break;
    }
    }

    if (_wrapper) {
        _wrapper->stopRecognition();
        _wrapper->unsubscribe(static_cast<IInputVolumeSubscriber *>(this));
        _wrapper->unsubscribe(
            static_cast<ITranscriptionResultSubscriber *>(this));
        _wrapper = nullptr;
    }

#ifdef Q_OS_WIN
    QFont font;
    if (locale == "zh_TW" || locale == "zh_HK") {
        font = QFont{"Microsoft Jhenghei", 10};
    } else if (locale == "zh_CN") {
        font = QFont{"Microsoft YaHei", 10};
    } else {
        font = QFont{"Microsoft YaHei", 10};
    }
    font.setStyleHint(QFont::System, QFont::PreferAntialias);
    _lineEdit->setFont(font);
    _lineEdit->clear();
#endif

#ifdef Q_OS_MAC
    _titleLabel->setText(tr("Listening..."));

    _wrapper = std::make_unique<TranscriberWrapper>(locale);
    _wrapper->subscribe(static_cast<IInputVolumeSubscriber *>(this));
    _wrapper->subscribe(static_cast<ITranscriptionResultSubscriber *>(this));
    _wrapper->startRecognition();

#endif
#ifdef Q_OS_WIN
    activateWindow();
    _lineEdit->setFocus();

    _wrapper = std::make_unique<TranscriberWrapper>(locale);
    QTimer::singleShot(1000, this, [&]() {
        _lineEdit->setFocus();
        _wrapper->subscribe(static_cast<IInputVolumeSubscriber *>(this));
        _wrapper->subscribe(static_cast<ITranscriptionResultSubscriber *>(this));
        _wrapper->startRecognition();
    });

#endif
}

void TranscriptionWindow::stopTranscription(void)
{
    if (_wrapper) {
        _wrapper->stopRecognition();
    }
}

void TranscriptionWindow::showErrorDialog(int err, std::string description)
{
    if (!isVisible()) {
        // Only show the error dialog once the transcription window has been painted
        QTimer::singleShot(100, this, [=, this]() {
            showErrorDialog(err, description);
        });
        return;
    }

    QString reason;

    switch (err) {
    case ENETDOWN: {
        reason = tr("Speech recognition service could not be started.");
        break;
    }
    case EPERM: {
        reason = tr(
            "Permission to access audio for speech recognition denied.");
        break;
    }
    case ENXIO: {
        reason = tr("No keyboard for the specified language could be found.");
        break;
    }
    default: {
        reason = tr("An unspecified error occurred.");
        break;
    }
    }

    _errorDialog = new TranscriptionErrorDialog(reason, description.c_str());
    _errorDialog->exec();
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
#ifdef Q_OS_MAC
    if (_titleLabel->text() != tr("Listening...")) {
        emit transcription(_titleLabel->text());
    }
#endif
#ifdef Q_OS_WIN
    emit transcription(_lineEdit->text());
#endif
    stopTranscription();
    close();
}

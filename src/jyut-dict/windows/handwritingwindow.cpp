#include "handwritingwindow.h"

#include "logic/settings/settings.h"
#include "logic/settings/settingsutils.h"
#include "logic/utils/utils_qt.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined(Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WINDOWS)
#include "logic/utils/utils_windows.h"
#endif

#include <QCoreApplication>
#include <QDesktopServices>
#include <QFont>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QSize>
#include <QStyle>
#include <QTimer>

namespace {
constexpr auto NUM_RESULTS = 10;
constexpr auto NUM_COLUMNS = 2;
constexpr auto NUM_RESULTS_PER_COLUMN = NUM_RESULTS / NUM_COLUMNS;
} // namespace

HandwritingWindow::HandwritingWindow(QWidget *parent)
    : QWidget{parent, Qt::Window}
{
    setObjectName("HandwritingWindow");
    _settings = Settings::getSettings();
    _handwritingWrapper = std::make_unique<HandwritingWrapper>(
        Handwriting::Script::TRADITIONAL);

    connect(_handwritingWrapper.get(),
            &HandwritingWrapper::recognizedResults,
            this,
            [&](std::vector<std::string> results) {
                for (int i = 0; i < results.size(); ++i) {
                    _buttons.at(i)->setText(
                        QString::fromStdString(results.at(i)));
                }
            });
    connect(_handwritingWrapper.get(),
            &HandwritingWrapper::modelError,
            this,
            [&](std::system_error &e) {
                showErrorDialog(e.code().value(), e.what());
            });

    Qt::WindowFlags flags = windowFlags() | Qt::CustomizeWindowHint
                            | Qt::WindowTitleHint;
    flags &= ~(Qt::WindowMinMaxButtonsHint | Qt::WindowFullscreenButtonHint);
    setWindowFlags(flags);
    setWindowModality(Qt::ApplicationModal);

    setupUI();
    translateUI();

    Handwriting::Script lastSelected
        = _settings
              ->value("Handwriting/lastSelected",
                      QVariant::fromValue(Handwriting::Script::TRADITIONAL))
              .value<Handwriting::Script>();
    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (const auto &button, buttons) {
        QVariant data = button->property("data");
        if (data.isValid()
            && data.value<Handwriting::Script>() == lastSelected) {
            button->click();
            break;
        }
    }
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
    switch (event->key()) {
    case Qt::Key_Escape: {
        if (_handwritingWrapper->strokesCleared()) {
            _doneButton->click();
        } else {
            _clearButton->click();
        }
        break;
    }
    case Qt::Key_Backspace:
        [[fallthrough]];
    case Qt::Key_Delete: {
        _backspaceButton->click();
        break;
    }
    case Qt::Key_Return:
        [[fallthrough]];
    case Qt::Key_1: {
        _buttons.at(0)->click();
        break;
    }
    case Qt::Key_2: {
        _buttons.at(1)->click();
        break;
    }
    case Qt::Key_3: {
        _buttons.at(2)->click();
        break;
    }
    case Qt::Key_4: {
        _buttons.at(3)->click();
        break;
    }
    case Qt::Key_5: {
        _buttons.at(4)->click();
        break;
    }
    case Qt::Key_6: {
        _buttons.at(5)->click();
        break;
    }
    case Qt::Key_7: {
        _buttons.at(6)->click();
        break;
    }
    case Qt::Key_8: {
        _buttons.at(7)->click();
        break;
    }
    case Qt::Key_9: {
        _buttons.at(8)->click();
        break;
    }
    case Qt::Key_0: {
        _buttons.at(9)->click();
        break;
    }
    case Qt::Key_T:
        [[fallthrough]];
    case Qt::Key_F:
        [[fallthrough]];
    case Qt::Key_Z: {
        _traditionalButton->click();
        break;
    }
    case Qt::Key_S:
        [[fallthrough]];
    case Qt::Key_G:
        [[fallthrough]];
    case Qt::Key_J: {
        _simplifiedButton->click();
        break;
    }
    };
}

void HandwritingWindow::setupUI()
{
#ifdef Q_OS_WIN
    _innerWidget = new QWidget{this};
#endif

    _layout = new QGridLayout{this};
    _layout->setSpacing(0);

    _traditionalButton = new QPushButton{this};
    _traditionalButton->setCheckable(true);
    _traditionalButton->setChecked(true);
    _traditionalButton->setObjectName("traditional");
    _traditionalButton->setProperty("scriptSelector", true);
    _traditionalButton->setProperty("data",
                                    QVariant::fromValue(
                                        Handwriting::Script::TRADITIONAL));
    connect(_traditionalButton,
            &QPushButton::clicked,
            this,
            &HandwritingWindow::setScript);
    _simplifiedButton = new QPushButton{this};
    _simplifiedButton->setCheckable(true);
    _simplifiedButton->setObjectName("simplified");
    _simplifiedButton->setProperty("scriptSelector", true);
    _simplifiedButton->setProperty("data",
                                   QVariant::fromValue(
                                       Handwriting::Script::SIMPLIFIED));
    connect(_simplifiedButton,
            &QPushButton::clicked,
            this,
            &HandwritingWindow::setScript);

    QWidget *scriptSelectorWidget = new QWidget();
    QHBoxLayout *selectorLayout = new QHBoxLayout{scriptSelectorWidget};

#ifdef Q_OS_MAC
    selectorLayout->setContentsMargins(0, 0, 0, 22);
#else
    selectorLayout->setContentsMargins(0, 0, 0, 6);
#endif
    scriptSelectorWidget->setLayout(selectorLayout);
    selectorLayout->addWidget(_traditionalButton);
    selectorLayout->addWidget(_simplifiedButton);

    _panel = new HandwritingPanel{this};
    _panel->setObjectName("HandwritingPanel");
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

    QWidget *spacer = new QWidget{this};
    spacer->setFixedWidth(_layout->contentsMargins().left());

    for (int i = 0; i < NUM_RESULTS; ++i) {
        _buttons.emplace_back(new QPushButton{this});
        _buttons.back()->setProperty("characterChoice", true);
        connect(_buttons.back(), &QPushButton::clicked, this, [&]() {
            _panel->clearPanel();
            _handwritingWrapper->clearStrokes();
            if (static_cast<QPushButton *>(sender())->text() != "　") {
                emit characterChosen(
                    static_cast<QPushButton *>(sender())->text());
            }
            for (const auto button : _buttons) {
                button->setText("　");
            }
        });
    }

    QWidget *clearButtonSpacer = new QWidget{this};
    clearButtonSpacer->setSizePolicy(QSizePolicy::MinimumExpanding,
                                     QSizePolicy::MinimumExpanding);

    _clearButton = new QPushButton{this};
    connect(_clearButton, &QPushButton::clicked, this, [&]() {
        _panel->clearPanel();
        _handwritingWrapper->clearStrokes();
        for (const auto button : _buttons) {
            button->setText("　");
        }
    });

    _backspaceButton = new QPushButton{this};
    connect(_backspaceButton, &QPushButton::clicked, this, [&]() {
        emit characterChosen(QString::fromLocal8Bit("\x8"));
    });

    _doneButton = new QPushButton{this};
    connect(_doneButton, &QPushButton::clicked, this, [&]() { close(); });

    QWidget *functionWidget = new QWidget{};
    QVBoxLayout *functionLayout = new QVBoxLayout{functionWidget};
#ifdef Q_OS_MAC
    functionLayout->setSpacing(11);
#else
    functionLayout->setSpacing(5);
#endif
    functionLayout->setContentsMargins(0, 22, 0, 0);
    functionWidget->setLayout(functionLayout);
    functionLayout->addWidget(_clearButton);
    functionLayout->addWidget(_backspaceButton);
    functionLayout->addWidget(_doneButton);

#ifdef Q_OS_WIN
    _innerWidget->setLayout(_layout);
    _outerWidgetLayout = new QGridLayout{this};
    _outerWidgetLayout->setContentsMargins(0, 0, 0, 0);
    _outerWidgetLayout->addWidget(_innerWidget);
#else
    setLayout(_layout);
#endif

    _layout->addWidget(scriptSelectorWidget, 0, 0, 1, -1);
    _layout->addWidget(_panel, 1, 0, NUM_RESULTS_PER_COLUMN + 2, 1);
    _layout->addWidget(spacer, 1, 1, NUM_RESULTS_PER_COLUMN + 2, 1);
    for (int column = 0; column < NUM_COLUMNS; ++column) {
        for (int row = 0; row < NUM_RESULTS_PER_COLUMN; ++row) {
            _layout->addWidget(_buttons.at(NUM_RESULTS_PER_COLUMN * column
                                           + row),
                               row + 1,
                               column + 2,
                               1,
                               1);
        }
    }

    _layout->addWidget(clearButtonSpacer,
                       NUM_RESULTS_PER_COLUMN + 1,
                       2,
                       1,
                       NUM_COLUMNS);
    _layout->setRowStretch(NUM_RESULTS_PER_COLUMN + 1, 1);
#ifdef Q_OS_MAC
    _layout->addWidget(functionWidget,
                       NUM_RESULTS_PER_COLUMN + 2,
                       2,
                       1,
                       NUM_COLUMNS,
                       Qt::AlignBaseline);
#else
    _layout->addWidget(functionWidget,
                       NUM_RESULTS_PER_COLUMN + 2,
                       2,
                       1,
                       NUM_COLUMNS);
#endif

    // Set the style to match whether the user started dark mode
    setStyle(Utils::isDarkMode());
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

    for (const auto button : _buttons) {
        button->setText("　");
    }

    _traditionalButton->setText(tr("Traditional Chinese"));
    _simplifiedButton->setText(tr("Simplified Chinese"));

    _clearButton->setText(tr("Clear"));
    _backspaceButton->setText(tr("Backspace"));
    _doneButton->setText(tr("Done"));

    setWindowTitle(tr("Handwriting"));

#ifndef Q_OS_LINUX
    layout()->setSizeConstraint(QLayout::SetFixedSize);
#endif
}

void HandwritingWindow::setStyle(bool use_dark)
{
#ifdef Q_OS_LINUX
    QColor borderColour = use_dark ? QColor{HEADER_BACKGROUND_COLOUR_DARK_R,
                                            HEADER_BACKGROUND_COLOUR_DARK_G,
                                            HEADER_BACKGROUND_COLOUR_DARK_B}
                                   : QColor{CONTENT_BACKGROUND_COLOUR_LIGHT_R,
                                            CONTENT_BACKGROUND_COLOUR_LIGHT_G,
                                            CONTENT_BACKGROUND_COLOUR_LIGHT_B};
    borderColour = borderColour.lighter(200);
#else
    QColor borderColour = use_dark ? QColor{HEADER_BACKGROUND_COLOUR_DARK_R,
                                            HEADER_BACKGROUND_COLOUR_DARK_G,
                                            HEADER_BACKGROUND_COLOUR_DARK_B}
                                   : QColor{HEADER_BACKGROUND_COLOUR_LIGHT_R,
                                            HEADER_BACKGROUND_COLOUR_LIGHT_G,
                                            HEADER_BACKGROUND_COLOUR_LIGHT_B};
#endif

    int interfaceSize = static_cast<int>(
        _settings
            ->value("Interface/size",
                    QVariant::fromValue(Settings::InterfaceSize::NORMAL))
            .value<Settings::InterfaceSize>());
#ifdef Q_OS_MAC
    int headerFontSize = Settings::h2FontSize.at(
        static_cast<unsigned long>(interfaceSize - 1));
#else
    int headerFontSize = Settings::h4FontSize.at(
        static_cast<unsigned long>(interfaceSize - 1));
#endif
    int bodyFontSize = Settings::bodyFontSize.at(
        static_cast<unsigned long>(interfaceSize - 1));
    int bodyFontSizeHan = Settings::bodyFontSizeHan.at(
        static_cast<unsigned long>(interfaceSize - 1));
    int borderRadius = static_cast<int>(bodyFontSize * 1);

    switch (_settings
                ->value("Interface/size",
                        QVariant::fromValue(Settings::InterfaceSize::NORMAL))
                .value<Settings::InterfaceSize>()) {
    case Settings::InterfaceSize::LARGE: {
        _panel->setFixedSize(375, 375);
        break;
    }
    case Settings::InterfaceSize::LARGER: {
        _panel->setFixedSize(400, 400);
        break;
    }
    default: {
        _panel->setFixedSize(350, 350);
    }
    }

#ifdef Q_OS_MAC
    QString style{"QLabel[isHan=\"true\"] { "
                  "   font-size: %1px; "
                  "} "
                  " "
                  "QLabel { "
                  "   font-size: %2px; "
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
                  "QWidget#HandwritingPanel { "
                  "   border: 1px solid %3; "
#ifdef Q_OS_WIN
                  "   border-radius: 5px; "
#endif
                  "} "};
#endif
    setStyleSheet(style.arg(std::to_string(bodyFontSizeHan).c_str(),
                            std::to_string(bodyFontSize).c_str()
#ifndef Q_OS_MAC
                                ,
                            borderColour.name()
#endif
                                ));

#ifdef Q_OS_WIN
    QString characterChoiceStyle = QString{"QPushButton { "
                                           "   background: palette(base); "
                                           "   border: 0px; "
                                           "   font-size: %1px; "
                                           "   margin: 0px; "
                                           "} "}
                                       .arg(headerFontSize);

#else
    QString characterChoiceStyle = QString{"QPushButton { "
                                           "   border: 0px; "
                                           "   font-size: %1px; "
                                           "} "}
                                       .arg(headerFontSize);
#endif

#ifdef Q_OS_MAC
    int padding = bodyFontSize / 3;
#else
    int padding = bodyFontSize / 6;
#endif
    int paddingHorizontal = bodyFontSize;
    QString buttonStyle = QString{"QPushButton { "
                                  "   background-color: transparent; "
#ifdef Q_OS_WIN
                                  "   border: 1px solid %1; "
#else
                                  "   border: 2px solid %1; "
#endif
                                  "   border-radius: %2px; "
                                  "   font-size: %3px; "
                                  "   padding: %4px; "
                                  "   padding-left: %5px; "
                                  "   padding-right: %5px; "
                                  "} "
                                  " "
                                  "QPushButton:checked { "
                                  "   background-color: %1; "
#ifdef Q_OS_WIN
                                  "   border: 1px solid %1; "
#else
                                  "   border: 2px solid %1; "
#endif
                                  "   border-radius: %2px; "
                                  "   font-size: %3px; "
                                  "   padding: %4px; "
                                  "   padding-left: %5px; "
                                  "   padding-right: %5px; "
                                  "} "
                                  " "
                                  "QPushButton:hover { "
                                  "   background-color: %1; "
#ifdef Q_OS_WIN
                                  "   border: 1px solid %1; "
#else
                                  "   border: 2px solid %1; "
#endif
                                  "   border-radius: %2px; "
                                  "   font-size: %3px; "
                                  "   padding: %4px; "
                                  "   padding-left: %5px; "
                                  "   padding-right: %5px; "
                                  "} "
                                  " "}
                              .arg(borderColour.name())
                              .arg(borderRadius)
                              .arg(bodyFontSize)
                              .arg(padding)
                              .arg(paddingHorizontal);

    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    int i = 0;
    foreach (const auto &button, buttons) {
        if (button->property("characterChoice").isValid()
            && button->property("characterChoice").toBool()) {
            ++i;
            button->setStyleSheet(characterChoiceStyle);
            button->ensurePolished();
#ifdef Q_OS_WIN
            button
                ->setFixedSize(button->fontMetrics().boundingRect("潑").height()
                                   + padding * 10,
                               button->fontMetrics().boundingRect("潑").width()
                                   + padding * 10);
#else
            button
                ->setFixedSize(button->fontMetrics().boundingRect("潑").height()
                                   + 2 * padding,
                               button->fontMetrics().boundingRect("潑").width()
                                   + 2 * padding);
#endif
            button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            button->ensurePolished();
        } else {
            button->setStyleSheet(buttonStyle);
            button->setMinimumHeight(std::max(
                borderRadius * 2,
                button->fontMetrics().boundingRect(button->text()).height()
                    + 2 * padding + 2 * 2));
        }
    }

#ifdef Q_OS_MAC
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet("QWidget#HandwritingWindow { "
                  "   background-color: palette(base); "
                  "} ");
#elif defined(Q_OS_WIN)
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
    for (const auto &button : _buttons) {
        button->setFont(font);
    }

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

void HandwritingWindow::setScript()
{
    QPushButton *sender = static_cast<QPushButton *>(QObject::sender());
    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (const auto &button, buttons) {
        button->setChecked(button == sender);
    }

    Settings::getSettings()->setValue("Handwriting/lastSelected",
                                      sender->property("data"));
    Handwriting::Script script
        = sender->property("data").value<Handwriting::Script>();
    _handwritingWrapper->setRecognizerScript(script);
    emit scriptSelected(script);
}

void HandwritingWindow::showErrorDialog(int err, std::string description)
{
    if (!isVisible()) {
        // Only show the error dialog once the handwriting window has been painted
        QTimer::singleShot(100, this, [=, this]() {
            showErrorDialog(err, description);
        });
        return;
    }

    QString reason;

    switch (err) {
    case ENOENT: {
        reason = tr("Handwriting models could not be copied.");
        break;
    }
    case ENOTDIR: {
        reason = tr("A folder for handwriting models could not be created.");
        break;
    }
    default: {
        reason = tr("An unspecified error occurred.");
        break;
    }
    }

    _errorDialog = new HandwritingErrorDialog(reason, description.c_str());
    _errorDialog->exec();
    close();
}

#include "loadingwidget.h"

#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QSize>
#include <QTimer>

LoadingWidget::LoadingWidget(QWidget *parent)
    : QWidget{parent}
{
    setupUI();
    translateUI();
    setStyle(Utils::isDarkMode());
}

LoadingWidget::~LoadingWidget()
{
    delete _movie;
}

void LoadingWidget::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, this, [=]() { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QWidget::changeEvent(event);
}

void LoadingWidget::setupUI(void)
{
    _movie = new QMovie{this};

    _movieLabel = new QLabel{this};
    _movieLabel->setAttribute(Qt::WA_TranslucentBackground, true);

    _descriptiveLabel = new QLabel{this};
    _descriptiveLabel->setObjectName("LoadingDescriptiveTextLabel");

    _layout = new QVBoxLayout{this};
    _layout->addWidget(_movieLabel);
    _layout->addWidget(_descriptiveLabel);

    _layout->setAlignment(_movieLabel, Qt::AlignHCenter);

    setContentsMargins(11, 11, 11, 0);
}

void LoadingWidget::translateUI(void)
{
    _descriptiveLabel->setText(tr("Searching for example sentences..."));
}

void LoadingWidget::setStyle(bool use_dark)
{
    QString textStyleSheet
        = "QLabel#LoadingDescriptiveTextLabel { color: %1; }";
    QColor textColour = use_dark ? QColor{LABEL_TEXT_COLOUR_DARK_R,
                                          LABEL_TEXT_COLOUR_DARK_G,
                                          LABEL_TEXT_COLOUR_DARK_B}
                                 : QColor{LABEL_TEXT_COLOUR_LIGHT_R,
                                          LABEL_TEXT_COLOUR_LIGHT_R,
                                          LABEL_TEXT_COLOUR_LIGHT_R};
    _descriptiveLabel->setStyleSheet(textStyleSheet.arg(textColour.name()));

    if (_movie) {
        delete _movie;
    }
    _movie = new QMovie{this};
    _movie->setFileName(use_dark ? ":/images/loading_inverted.gif"
                                 : ":/images/loading.gif");
    _movie->setScaledSize(QSize{25, 25});
    _movie->start();
    _movieLabel->setMovie(_movie);
}

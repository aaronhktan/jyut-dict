#include "definitioncontentwidget.h"

#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined(Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QTimer>

DefinitionContentWidget::DefinitionContentWidget(QWidget *parent) : QWidget(parent)
{
    _definitionLayout = new QGridLayout{this};
    _definitionLayout->setContentsMargins(10, 0, 10, 10);

    _definitionNumberLabels = {};
    _definitionLabels = {};
}

DefinitionContentWidget::~DefinitionContentWidget()
{
    cleanupLabels();
}

void DefinitionContentWidget::changeEvent(QEvent *event)
{
#if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, [=]() { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
#endif
    QWidget::changeEvent(event);
}

void DefinitionContentWidget::setEntry(std::vector<Definition::Definition> definitions)
{
    cleanupLabels();

    for (size_t i = 0; i < definitions.size(); i++) {
        std::string number = std::to_string(i + 1);
        _definitionNumberLabels.push_back(new QLabel{number.c_str(), this});
        int definitionNumberWidth = _definitionNumberLabels.back()
                                        ->fontMetrics()
                                        .boundingRect("PY")
                                        .width();
        _definitionNumberLabels.back()->setFixedWidth(definitionNumberWidth);
        int definitionNumberHeight = _definitionNumberLabels.back()
                                         ->fontMetrics()
                                         .boundingRect("123PYing")
                                         .height();
        _definitionNumberLabels.back()->setFixedHeight(definitionNumberHeight);

        _definitionLabels.push_back(new QLabel{definitions[i].definitionContent.c_str(), this});
        _definitionLabels.back()->setWordWrap(true);
        _definitionLabels.back()->setTextInteractionFlags(Qt::TextSelectableByMouse);

        _definitionLayout->addWidget(_definitionNumberLabels[i],
                                     static_cast<int>(i + 9), 0, Qt::AlignTop);
        _definitionLayout->addWidget(_definitionLabels[i],
                                     static_cast<int>(i + 9), 1, Qt::AlignTop);
    }

#if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    setStyle(Utils::isDarkMode());
#else
    setStyle(/* use_dark = */false);
#endif
}

void DefinitionContentWidget::setStyle(bool use_dark)
{
    QString styleSheet = "QLabel { color: %1; }";
    QColor textColour = use_dark ? QColor{LABEL_TEXT_COLOUR_DARK_R,
                                          LABEL_TEXT_COLOUR_DARK_G,
                                          LABEL_TEXT_COLOUR_DARK_B}
                                 : QColor{LABEL_TEXT_COLOUR_LIGHT_R,
                                          LABEL_TEXT_COLOUR_LIGHT_R,
                                          LABEL_TEXT_COLOUR_LIGHT_R};
    for (auto label : _definitionNumberLabels) {
        label->setStyleSheet(styleSheet.arg(textColour.name()));
    }
}

void DefinitionContentWidget::cleanupLabels()
{
    for (auto label : _definitionNumberLabels) {
        _definitionLayout->removeWidget(label);
        delete label;
    }
    _definitionNumberLabels.clear();

    for (auto label : _definitionLabels) {
        _definitionLayout->removeWidget(label);
        delete label;
    }
    _definitionLabels.clear();
}

#include "definitioncontentwidget.h"

#include "logic/settings/settingsutils.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined(Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QTimer>

DefinitionContentWidget::DefinitionContentWidget(QWidget *parent) : QWidget(parent)
{
    _definitionLayout = new QGridLayout{this};
    _definitionLayout->setVerticalSpacing(1);
    _definitionLayout->setContentsMargins(10, 0, 10, 0);
}

DefinitionContentWidget::~DefinitionContentWidget()
{
    cleanupLabels();
}

void DefinitionContentWidget::changeEvent(QEvent *event)
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
        setStyle(Utils::isDarkMode());
    }
    QWidget::changeEvent(event);
}

void DefinitionContentWidget::setEntry(const std::vector<Definition::Definition> &definitions)
{
    cleanupLabels();

    int rowNumber = 0;
    for (size_t i = 0; i < definitions.size(); i++) {
        std::string number = std::to_string(i + 1);

        _definitionNumberLabels.push_back(new QLabel{number.c_str(), this});
        int definitionNumberWidth = _definitionNumberLabels.back()
                                        ->fontMetrics()
                                        .boundingRect("999")
                                        .width();
        _definitionNumberLabels.back()->setFixedWidth(definitionNumberWidth);
        int definitionNumberHeight = _definitionNumberLabels.back()
                                         ->fontMetrics()
                                         .boundingRect("123PYing")
                                         .height();
        _definitionNumberLabels.back()->setFixedHeight(definitionNumberHeight);
        _definitionLayout->addWidget(_definitionNumberLabels.back(),
                                     static_cast<int>(rowNumber),
                                     0,
                                     Qt::AlignTop);

        QString label{definitions[i].label.c_str()};
        if (!label.isEmpty()) {
            _definitionLabelLabels.push_back(
                new QLabel{definitions[i].label.c_str(), this});
            _definitionLabelLabels.back()->setWordWrap(true);
            _definitionLayout->addWidget(_definitionLabelLabels.back(),
                                         static_cast<int>(rowNumber++),
                                         1,
                                         Qt::AlignTop);
        }

        _definitionLabels.push_back(
            new QLabel{definitions[i].definitionContent.c_str(), this});
        _definitionLabels.back()->setWordWrap(true);
        _definitionLabels.back()->setTextInteractionFlags(Qt::TextSelectableByMouse);
        _definitionLayout->addWidget(_definitionLabels.back(),
                                     static_cast<int>(rowNumber++), 1, Qt::AlignTop);

        for (size_t j = 0; j < definitions[i].sentences.size(); j++) {
            QString exampleText;
            switch (Settings::getSettings()
                        ->value("characterOptions",
                                QVariant::fromValue(
                                    EntryCharactersOptions::PREFER_TRADITIONAL))
                        .value<EntryCharactersOptions>()) {
            case EntryCharactersOptions::ONLY_SIMPLIFIED:
                exampleText = definitions[i].sentences[j].getSimplified().c_str();
                break;
            case EntryCharactersOptions::PREFER_SIMPLIFIED:
                exampleText
                    = QString{definitions[i].sentences[j].getSimplified().c_str()}
                      + "<br>"
                      + QString{
                          definitions[i].sentences[j].getTraditional().c_str()};
                break;
            case EntryCharactersOptions::ONLY_TRADITIONAL:
                exampleText
                    = definitions[i].sentences[j].getTraditional().c_str();
                break;
            case EntryCharactersOptions::PREFER_TRADITIONAL:
                exampleText
                    = QString{definitions[i].sentences[j].getTraditional().c_str()}
                      + "<br>"
                      + QString{
                          definitions[i].sentences[j].getSimplified().c_str()};
                break;
            }

            _exampleLabels.push_back(
                new QLabel{"<ul style=\"list-style-type:circle;\"><li>"
                               + exampleText + "</li></ul>",
                           this});
            _exampleLabels.back()->setContentsMargins(0, 0, 0, 0);
            _exampleLabels.back()->setWordWrap(true);
            _exampleLabels.back()->setTextInteractionFlags(
                Qt::TextSelectableByMouse);
            _definitionLayout->addWidget(_exampleLabels.back(),
                                         static_cast<int>(rowNumber++),
                                         1,
                                         Qt::AlignTop);

            QString pronunciationText;
            QString cantonese
                = QString{definitions[i]
                              .sentences[j]
                              .getCantonesePhonetic(
                                  Settings::getSettings()
                                      ->value("cantoneseOptions",
                                              QVariant::fromValue(
                                                  CantoneseOptions::RAW_JYUTPING))
                                      .value<CantoneseOptions>())
                              .c_str()}
                      .trimmed();
            QString mandarin
                = QString{definitions[i]
                              .sentences[j]
                              .getMandarinPhonetic(
                                  Settings::getSettings()
                                      ->value("mandarinOptions",
                                              QVariant::fromValue(
                                                  MandarinOptions::PRETTY_PINYIN))
                                      .value<MandarinOptions>())
                              .c_str()}
                      .trimmed();

            switch (Settings::getSettings()
                        ->value("phoneticOptions",
                                QVariant::fromValue(
                                    EntryPhoneticOptions::PREFER_CANTONESE))
                        .value<EntryPhoneticOptions>()) {
            case EntryPhoneticOptions::ONLY_CANTONESE:
                pronunciationText = cantonese;
                break;
            case EntryPhoneticOptions::PREFER_CANTONESE:
                pronunciationText = cantonese
                                    + (cantonese.isEmpty() || mandarin.isEmpty()
                                           ? ""
                                           : "<br>")
                                    + mandarin;
                break;
            case EntryPhoneticOptions::ONLY_MANDARIN:
                pronunciationText = mandarin;
                break;
            case EntryPhoneticOptions::PREFER_MANDARIN:
                pronunciationText = mandarin
                                    + (cantonese.isEmpty() || mandarin.isEmpty()
                                           ? ""
                                           : "<br>")
                                    + cantonese;
                break;
            }

            if (!pronunciationText.isEmpty()) {
                _examplePronunciationLabels.push_back(
                    new QLabel{"<ul style=\"list-style-type:none;\"><li>"
                                   + pronunciationText + "</li></ul>",
                               this});
                _examplePronunciationLabels.back()->setContentsMargins(0, 0, 0, 0);
                _examplePronunciationLabels.back()->setWordWrap(true);
                _examplePronunciationLabels.back()->setTextInteractionFlags(
                    Qt::TextSelectableByMouse);
                _definitionLayout->addWidget(_examplePronunciationLabels.back(),
                                             static_cast<int>(rowNumber++),
                                             1,
                                             Qt::AlignTop);
            }

            auto sets = definitions[i].sentences[j].getSentenceSets();
            if (!sets.empty()) {
                auto set = sets[0].getSentences();
                if (!set.empty()) {
                    QString translation = set[0].sentence.c_str();
                    _exampleTranslationLabels.push_back(
                        new QLabel{"<ul style=\"list-style-type:none;\"><li>"
                                       + translation + "</li></ul>",
                                   this});
                    _exampleTranslationLabels.back()->setWordWrap(true);
                    _exampleTranslationLabels.back()->setTextInteractionFlags(
                        Qt::TextSelectableByMouse);
                    _definitionLayout->addWidget(_exampleTranslationLabels.back(),
                                                 static_cast<int>(rowNumber++),
                                                 1,
                                                 Qt::AlignTop);
                }
            }
        }

        // This label adds space between definitions in the definition card.
        _spaceLabels.push_back(new QLabel{" "});
        _spaceLabels.back()->setMaximumHeight(4);
        _definitionLayout->addWidget(_spaceLabels.back(),
                                     static_cast<int>(rowNumber++),
                                     1,
                                     Qt::AlignTop);
    }

    // This label adds a bit of space at the end of the definition card.
    _spaceLabels.push_back(new QLabel{" "});
    _spaceLabels.back()->setMaximumHeight(6);
    _definitionLayout->addWidget(_spaceLabels.back(),
                                 static_cast<int>(rowNumber++),
                                 1,
                                 Qt::AlignTop);

    setStyle(Utils::isDarkMode());
}

void DefinitionContentWidget::setStyle(bool use_dark)
{
    QColor textColour = use_dark ? QColor{LABEL_TEXT_COLOUR_DARK_R,
                                          LABEL_TEXT_COLOUR_DARK_G,
                                          LABEL_TEXT_COLOUR_DARK_B}
                                 : QColor{LABEL_TEXT_COLOUR_LIGHT_R,
                                          LABEL_TEXT_COLOUR_LIGHT_R,
                                          LABEL_TEXT_COLOUR_LIGHT_R};
    QString definitionNumberStyleSheet = "QLabel { color: %1; "
                                         "margin-top: 2px; }";
    for (const auto &label : _definitionNumberLabels) {
        label->setStyleSheet(definitionNumberStyleSheet.arg(textColour.name()));
    }
    QString definitionLabelStyleSheet = "QLabel { color: %1; "
                                        "font-style: italic; "
                                        "text-transform: lowercase; }";
    for (const auto &label : _definitionLabelLabels) {
        label->setStyleSheet(definitionLabelStyleSheet.arg(textColour.name()));
    }

    QString examplePronunciationStyleSheet = "QLabel { color: %1; "
                                             "padding-left: 0px; "
                                             "margin-left: 0px; } ";
    for (const auto &label : _examplePronunciationLabels) {
        label->setStyleSheet(
            examplePronunciationStyleSheet.arg(textColour.name()));
    }

    QString translationStyleSheet = "QLabel { color: %1; "
                                    "font-style: italic; } ";
    for (const auto &label : _exampleTranslationLabels) {
        label->setStyleSheet(translationStyleSheet.arg(textColour.name()));
    }

#ifdef Q_OS_WIN
    QFont font = QFont{"Microsoft YaHei", 10};
    font.setStyleHint(QFont::System, QFont::PreferAntialias);
    for (const auto &label : _definitionLabelLabels) {
        label->setFont(font);
    }
    for (const auto &label : _definitionLabels) {
        label->setFont(font);
    }
    for (const auto &label : _exampleLabels) {
        label->setFont(font);
    }
    for (const auto &label : _exampleTranslationLabels) {
        label->setFont(font);
    }
#endif
}

void DefinitionContentWidget::cleanupLabels()
{
    for (const auto &label : _definitionNumberLabels) {
        _definitionLayout->removeWidget(label);
        delete label;
    }
    _definitionNumberLabels.clear();

    for (const auto &label : _definitionLabelLabels) {
        _definitionLayout->removeWidget(label);
        delete label;
    }
    _definitionLabelLabels.clear();

    for (const auto &label : _definitionLabels) {
        _definitionLayout->removeWidget(label);
        delete label;
    }
    _definitionLabels.clear();

    for (const auto &label : _exampleLabels) {
        _definitionLayout->removeWidget(label);
        delete label;
    }
    _exampleLabels.clear();

    for (const auto &label : _spaceLabels) {
        _definitionLayout->removeWidget(label);
        delete label;
    }
    _spaceLabels.clear();
}

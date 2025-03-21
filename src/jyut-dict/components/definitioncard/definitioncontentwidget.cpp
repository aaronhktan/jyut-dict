#include "definitioncontentwidget.h"

#include "logic/settings/settings.h"
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
    _settings = Settings::getSettings(this);
    _definitionLayout = new QGridLayout{this};
    _definitionLayout->setVerticalSpacing(1);
    _definitionLayout->setContentsMargins(10, 0, 10, 0);
    _definitionLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
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
        QTimer::singleShot(10, this, [=, this]() { _paletteRecentlyChanged = false; });

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
        _definitionLayout->addWidget(_definitionNumberLabels.back(),
                                     static_cast<int>(rowNumber),
                                     0,
                                     Qt::AlignTop);

        QString label = QString::fromStdString(definitions[i].label);
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
        _definitionLabels.back()->setTextInteractionFlags(
            Qt::TextSelectableByMouse);
        _definitionLayout->addWidget(_definitionLabels.back(),
                                     static_cast<int>(rowNumber++),
                                     1,
                                     Qt::AlignTop);

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
                exampleText = QString::fromStdString(
                                  definitions[i].sentences[j].getSimplified())
                              + "<br>"
                              + QString::fromStdString(
                                  definitions[i].sentences[j].getTraditional());
                break;
            case EntryCharactersOptions::ONLY_TRADITIONAL:
                exampleText
                    = definitions[i].sentences[j].getTraditional().c_str();
                break;
            case EntryCharactersOptions::PREFER_TRADITIONAL:
                exampleText = QString::fromStdString(
                                  definitions[i].sentences[j].getTraditional())
                              + "<br>"
                              + QString::fromStdString(
                                  definitions[i].sentences[j].getSimplified());
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

            CantoneseOptions cantoneseOptions
                = Settings::getSettings()
                      ->value("Preview/cantonesePronunciationOptions",
                              QVariant::fromValue(
                                  CantoneseOptions::RAW_JYUTPING))
                      .value<CantoneseOptions>();

            MandarinOptions mandarinOptions
                = Settings::getSettings()
                      ->value("Preview/mandarinPronunciationOptions",
                              QVariant::fromValue(
                                  MandarinOptions::PRETTY_PINYIN))
                      .value<MandarinOptions>();

            SourceSentence sentence = definitions[i].sentences[j];
            sentence.generatePhonetic(cantoneseOptions, mandarinOptions);

            QString cantonese = QString::fromStdString(
                                    sentence.getCantonesePhonetic(
                                        cantoneseOptions))
                                    .trimmed();
            QString mandarin = QString::fromStdString(
                                   sentence.getMandarinPhonetic(mandarinOptions))
                                   .trimmed();

            switch (Settings::getSettings()
                        ->value("Preview/phoneticOptions",
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
                // Don't use the "list-style-type:none;" stylesheet here; for some
                // reason it changes all double-spaces (e.g. "  ") to single spaces
                _examplePronunciationLabels.push_back(
                    new QLabel{pronunciationText, this});
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
                    QString translation = QString::fromStdString(
                        set[0].sentence);
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
    int interfaceSize = static_cast<int>(
        _settings
            ->value("Interface/size",
                    QVariant::fromValue(Settings::InterfaceSize::NORMAL))
            .value<Settings::InterfaceSize>());
    int bodyFontSize = Settings::bodyFontSize.at(
        static_cast<unsigned long>(interfaceSize - 1));
    int bodyFontSizeHan = Settings::bodyFontSizeHan.at(
        static_cast<unsigned long>(interfaceSize - 1));

    QString definitionNumberStyleSheet = "QLabel { "
                                         "   color: %1; "
                                         "   font-size: %2px; "
                                         "}";
    for (const auto &label : _definitionNumberLabels) {
        label->setStyleSheet(
            definitionNumberStyleSheet.arg(textColour.name()).arg(bodyFontSize));
    }
    QString definitionLabelLabelStyleSheet = "QLabel { "
                                             "   color: %1; "
                                             "   font-size: %2px; "
                                             "   font-style: italic; "
                                             "   text-transform: lowercase; "
                                             "}";
    for (const auto &label : _definitionLabelLabels) {
        label->setStyleSheet(
            definitionLabelLabelStyleSheet.arg(textColour.name())
                .arg(bodyFontSize));
    }
    QString definitionLabelStyleSheet = "QLabel { "
                                        "   font-size: %2px; "
                                        "}";
    for (const auto &label : _definitionLabels) {
        label->setStyleSheet(definitionLabelStyleSheet.arg(bodyFontSize));
    }

    QString exampleStyleSheet = "QLabel { "
                                "   font-size: %2px; "
                                "}";
    for (const auto &label : _exampleLabels) {
        label->setStyleSheet(exampleStyleSheet.arg(bodyFontSizeHan));
    }
    QString examplePronunciationStyleSheet = "QLabel { "
                                             "   color: %1; "
                                             "   font-size: %2px; "
                                             "   padding-left: 38px; "
                                             "   margin-left: 0px; "
                                             "} ";
    for (const auto &label : _examplePronunciationLabels) {
        label->setStyleSheet(
            examplePronunciationStyleSheet.arg(textColour.name())
                .arg(bodyFontSize));
    }
    QString translationStyleSheet = "QLabel { "
                                    "   color: %1; "
                                    "   font-size: %2px; "
                                    "   font-style: italic; "
                                    "} ";
    for (const auto &label : _exampleTranslationLabels) {
        label->setStyleSheet(
            translationStyleSheet.arg(textColour.name()).arg(bodyFontSize));
    }

#ifdef Q_OS_WIN
    QFont font = QFont{"Microsoft YaHei"};
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

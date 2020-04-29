#include "entryheaderwidget.h"

#include "logic/settings/settingsutils.h"
#include "logic/strings/strings.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QCoreApplication>
#include <QTimer>
#include <QVariant>

EntryHeaderWidget::EntryHeaderWidget(QWidget *parent) : QWidget(parent)
{
    _entryHeaderLayout = new QGridLayout{this};
    _entryHeaderLayout->setContentsMargins(0, 0, 0, 0);

    _wordLabel = new QLabel{this};
    _wordLabel->setStyleSheet("QLabel { font-size: 30px }");
    _wordLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    _wordLabel->setWordWrap(true);
#ifdef Q_OS_WIN
    _wordLabel->setFont(QFont("Microsoft YaHei"));
#endif

    _jyutpingLabel = new QLabel{this};
    _jyutpingPronunciation = new QLabel{this};
    _jyutpingPronunciation->setTextInteractionFlags(Qt::TextSelectableByMouse);
    _jyutpingLabel->setVisible(false);
    _jyutpingPronunciation->setWordWrap(true);


    _pinyinLabel = new QLabel{this};
    _pinyinPronunciation = new QLabel{this};
    _pinyinPronunciation->setTextInteractionFlags(Qt::TextSelectableByMouse);
    _pinyinLabel->setVisible(false);
    _pinyinPronunciation->setWordWrap(true);

    _entryHeaderLayout->addWidget(_wordLabel, 1, 0, 1, -1);
    _entryHeaderLayout->addWidget(_jyutpingLabel, 2, 0, 1, 1, Qt::AlignTop);
    _entryHeaderLayout->addWidget(_jyutpingPronunciation, 2, 1, 1, 1);
    _entryHeaderLayout->addWidget(_pinyinLabel, 3, 0, 1, 1, Qt::AlignTop);
    _entryHeaderLayout->addWidget(_pinyinPronunciation, 3, 1, 1, 1);

#ifdef Q_OS_MAC
    setStyle(Utils::isDarkMode());
#else
    setStyle(/* use_dark = */false);
#endif
    translateUI();
}

EntryHeaderWidget::~EntryHeaderWidget()
{

}

void EntryHeaderWidget::changeEvent(QEvent *event)
{
#ifdef Q_OS_WIN
    if (event->type() == QEvent::FontChange) {
        _jyutpingLabel->setFixedWidth(_jyutpingLabel->fontMetrics().boundingRect("JP").width());
        _pinyinLabel->setFixedWidth(_pinyinLabel->fontMetrics().boundingRect("PY").width());
    }
#endif
#if defined(Q_OS_DARWIN)
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(100, [=]() { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
#endif
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QWidget::changeEvent(event);
}

void EntryHeaderWidget::setEntry(const Entry &entry)
{
    _jyutpingLabel->setVisible(true);
    _pinyinLabel->setVisible(true);

    _wordLabel->setText(
        entry
            .getCharacters(
                Settings::getSettings()
                    ->value("characterOptions",
                            QVariant::fromValue(
                                EntryCharactersOptions::PREFER_TRADITIONAL))
                    .value<EntryCharactersOptions>(),
                true)
            .c_str());

    _jyutpingPronunciation->setText(
        entry
            .getCantonesePhonetic(
                Settings::getSettings()
                    ->value("cantoneseOptions",
                            QVariant::fromValue(CantoneseOptions::RAW_JYUTPING))
                    .value<CantoneseOptions>())
            .c_str());
    _pinyinPronunciation->setText(
        entry
            .getMandarinPhonetic(
                Settings::getSettings()
                    ->value("mandarinOptions",
                            QVariant::fromValue(MandarinOptions::PRETTY_PINYIN))
                    .value<MandarinOptions>())
            .c_str());

    displayPronunciationLabels(
        Settings::getSettings()
            ->value("phoneticOptions",
                    QVariant::fromValue(EntryPhoneticOptions::PREFER_JYUTPING))
            .value<EntryPhoneticOptions>());
}

void EntryHeaderWidget::setEntry(std::string word,
                                 std::string jyutping, std::string pinyin)
{
    _jyutpingLabel->setVisible(true);
    _pinyinLabel->setVisible(true);

    _wordLabel->setText(word.c_str());
    _jyutpingPronunciation->setText(jyutping.c_str());
    _pinyinPronunciation->setText(pinyin.c_str());
}


void EntryHeaderWidget::setStyle(bool use_dark)
{
    QString styleSheet = "QLabel { color: %1; }";
    QColor textColour = use_dark ? QColor{LABEL_TEXT_COLOUR_DARK_R,
                                          LABEL_TEXT_COLOUR_DARK_G,
                                          LABEL_TEXT_COLOUR_DARK_B}
                                 : QColor{LABEL_TEXT_COLOUR_LIGHT_R,
                                          LABEL_TEXT_COLOUR_LIGHT_R,
                                          LABEL_TEXT_COLOUR_LIGHT_R};
    _jyutpingLabel->setStyleSheet(styleSheet.arg(textColour.name()));
    _pinyinLabel->setStyleSheet(styleSheet.arg(textColour.name()));
}

void EntryHeaderWidget::translateUI()
{
    _jyutpingLabel->setText(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                                        Strings::JYUTPING_SHORT));
    _jyutpingLabel->setFixedWidth(
        _jyutpingLabel->fontMetrics().boundingRect(_jyutpingLabel->text()).width());

    _pinyinLabel->setText(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                                      Strings::PINYIN_SHORT));
    _pinyinLabel->setFixedWidth(
        _pinyinLabel->fontMetrics().boundingRect(_pinyinLabel->text()).width());
}

void EntryHeaderWidget::displayPronunciationLabels(const EntryPhoneticOptions options)
{
    switch (options) {
        case EntryPhoneticOptions::PREFER_JYUTPING: {
            _entryHeaderLayout->addWidget(_jyutpingLabel, 2, 0, 1, 1, Qt::AlignTop);
            _entryHeaderLayout->addWidget(_jyutpingPronunciation, 2, 1, 1, 1);
            _entryHeaderLayout->addWidget(_pinyinLabel, 3, 0, 1, 1, Qt::AlignTop);
            _entryHeaderLayout->addWidget(_pinyinPronunciation, 3, 1, 1, 1);
            _pinyinLabel->setVisible(true);
            _pinyinPronunciation->setVisible(true);
            _jyutpingLabel->setVisible(true);
            _jyutpingPronunciation->setVisible(true);
            break;
        }
        case EntryPhoneticOptions::PREFER_PINYIN: {
            _entryHeaderLayout->addWidget(_pinyinLabel, 2, 0, 1, 1, Qt::AlignTop);
            _entryHeaderLayout->addWidget(_pinyinPronunciation, 2, 1, 1, 1);
            _entryHeaderLayout->addWidget(_jyutpingLabel, 3, 0, 1, 1, Qt::AlignTop);
            _entryHeaderLayout->addWidget(_jyutpingPronunciation, 3, 1, 1, 1);
            _pinyinLabel->setVisible(true);
            _pinyinPronunciation->setVisible(true);
            _jyutpingLabel->setVisible(true);
            _jyutpingPronunciation->setVisible(true);
            break;
        }
        case EntryPhoneticOptions::ONLY_JYUTPING: {
            _jyutpingLabel->setVisible(true);
            _jyutpingPronunciation->setVisible(true);
            _pinyinLabel->setVisible(false);
            _pinyinPronunciation->setVisible(false);
            break;
        }
        case EntryPhoneticOptions::ONLY_PINYIN: {
            _jyutpingLabel->setVisible(false);
            _jyutpingPronunciation->setVisible(false);
            _pinyinLabel->setVisible(true);
            _pinyinPronunciation->setVisible(true);
            break;
        }
    }
}

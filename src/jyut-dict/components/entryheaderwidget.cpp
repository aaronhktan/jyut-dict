#include "entryheaderwidget.h"

EntryHeaderWidget::EntryHeaderWidget(QWidget *parent) : QWidget(parent)
{
    _entryHeaderLayout = new QGridLayout(this);
    _entryHeaderLayout->setContentsMargins(0, 0, 0, 0);

    _wordLabel = new QLabel(this);
    _wordLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    _wordLabel->setWordWrap(true);
    _wordLabel->setStyleSheet("QLabel { font-size: 24px }");
#ifdef Q_OS_WIN
    _wordLabel->setFont(QFont("Microsoft YaHei"));
#endif

    _jyutpingLabel = new QLabel("<font color=#6f6f6f>JP</font>", this);
    _jyutpingLabel->setFixedWidth(_jyutpingLabel->fontMetrics().boundingRect("JP").width());
    _jyutpingPronunciation = new QLabel(this);
    _jyutpingPronunciation->setWordWrap(true);
    _jyutpingLabel->setVisible(false);

    _pinyinLabel = new QLabel("<font color=#6f6f6f>PY</font>");
    _pinyinLabel->setFixedWidth(_pinyinLabel->fontMetrics().boundingRect("PY").width());
    _pinyinPronunciation = new QLabel(this);
    _pinyinPronunciation->setWordWrap(true);
    _pinyinLabel->setVisible(false);

    _entryHeaderLayout->addWidget(_wordLabel, 1, 0, 1, -1);
    _entryHeaderLayout->addWidget(_jyutpingLabel, 2, 0, 1, 1, Qt::AlignTop);
    _entryHeaderLayout->addWidget(_jyutpingPronunciation, 2, 1, 1, 1);
    _entryHeaderLayout->addWidget(_pinyinLabel, 3, 0, 1, 1, Qt::AlignTop);
    _entryHeaderLayout->addWidget(_pinyinPronunciation, 3, 1, 1, 1);

    setLayout(_entryHeaderLayout);
}

EntryHeaderWidget::~EntryHeaderWidget()
{

}

void EntryHeaderWidget::setEntry(Entry& entry)
{
    _jyutpingLabel->setVisible(true);
    _pinyinLabel->setVisible(true);

    _wordLabel->setText(entry.getCharacters(EntryCharactersOptions::PREFER_TRADITIONAL, true).c_str());
    _jyutpingPronunciation->setText(entry.getJyutping().c_str());
    _pinyinPronunciation->setText(entry.getPrettyPinyin().c_str());
}

void EntryHeaderWidget::setEntry(std::string word, std::string jyutping, std::string pinyin)
{
    _jyutpingLabel->setVisible(true);
    _pinyinLabel->setVisible(true);

    _wordLabel->setText(word.c_str());
    _jyutpingPronunciation->setText(jyutping.c_str());
    _pinyinPronunciation->setText(pinyin.c_str());
}

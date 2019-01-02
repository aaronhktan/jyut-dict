#ifndef ENTRYHEADERWIDGET_H
#define ENTRYHEADERWIDGET_H

#include <logic/entry/entry.h>

#include <QGridLayout>
#include <QLabel>

class EntryHeaderWidget : public QWidget
{
public:
    EntryHeaderWidget(QWidget *parent = nullptr);

    void setEntry(Entry& entry);
    void setEntry(std::string word, std::string jyutping, std::string pinyin);

private:
    QGridLayout *_entryHeaderLayout;
    QLabel *_wordLabel;
    QLabel *_jyutpingLabel;
    QLabel *_jyutpingPronunciation;
    QLabel *_pinyinLabel;
    QLabel *_pinyinPronunciation;
};

#endif // ENTRYHEADERWIDGET_H

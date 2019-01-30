#ifndef ENTRYHEADERWIDGET_H
#define ENTRYHEADERWIDGET_H

#include <logic/entry/entry.h>

#include <QGridLayout>
#include <QLabel>

// The EntryHeaderWidget displays basic information about the entry
// at the top of the detail view

class EntryHeaderWidget : public QWidget
{
public:
    EntryHeaderWidget(QWidget *parent = nullptr);
    ~EntryHeaderWidget();

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

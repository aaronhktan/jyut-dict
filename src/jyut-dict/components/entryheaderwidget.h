#ifndef ENTRYHEADERWIDGET_H
#define ENTRYHEADERWIDGET_H

#include "logic/entry/entry.h"
#include "logic/entry/entryphoneticoptions.h"

#include <QEvent>
#include <QGridLayout>
#include <QLabel>
#include <QWidget>

#include <string>

// The EntryHeaderWidget displays basic information about the entry
// at the top of the detail view

class EntryHeaderWidget : public QWidget
{
public:
    explicit EntryHeaderWidget(QWidget *parent = nullptr);
    ~EntryHeaderWidget() override;

    void changeEvent(QEvent *event) override;
//    QSize sizeHint() const override;

    void setEntry(const Entry &entry);
    void setEntry(std::string word, std::string jyutping, std::string pinyin);

private:
    void displayPronunciationLabels(const EntryPhoneticOptions options);

    QGridLayout *_entryHeaderLayout;
    QLabel *_wordLabel;
    QLabel *_jyutpingLabel;
    QLabel *_jyutpingPronunciation;
    QLabel *_pinyinLabel;
    QLabel *_pinyinPronunciation;
};

#endif // ENTRYHEADERWIDGET_H

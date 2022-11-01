#ifndef ENTRYHEADERWIDGET_H
#define ENTRYHEADERWIDGET_H

#include "dialogs/entryspeakerrordialog.h"
#include "logic/entry/entry.h"
#include "logic/entry/entryphoneticoptions.h"
#include "logic/entry/entryspeaker.h"
#include "logic/settings/settingsutils.h"

#include <QEvent>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include <string>

// The EntryHeaderWidget displays basic information about the entry
// at the top of the entry detail view

class EntryHeaderWidget : public QWidget
{
public:
    explicit EntryHeaderWidget(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

    void setEntry(const Entry &entry);

private:
    void setStyle(bool use_dark);
    void translateUI();

    void displayPronunciationLabels(const CantoneseOptions &cantoneseOptions,
                                    const MandarinOptions &mandarinOptions) const;

    void showError(const QString &reason, const QString &message);

    bool _paletteRecentlyChanged = false;

    QString _chinese;
    QString _jyutping;
    QString _pinyin;
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    std::unique_ptr<EntrySpeaker> _speaker;
#endif

    // TODO: Refactor this into a vector of labels so I don't need to make specific
    // labels for specific kinds of romanizations
    QGridLayout *_entryHeaderLayout;
    QLabel *_wordLabel;
    QLabel *_jyutpingLabel;
    QPushButton *_jyutpingTTS;
    QLabel *_jyutpingPronunciation;
    QLabel *_yaleLabel;
    QPushButton *_yaleTTS;
    QLabel *_yalePronunciation;
    QLabel *_pinyinLabel;
    QPushButton *_pinyinTTS;
    QLabel *_pinyinPronunciation;

    EntrySpeakErrorDialog *_message;
};

#endif // ENTRYHEADERWIDGET_H

#ifndef SENTENCEVIEWHEADERWIDGET_H
#define SENTENCEVIEWHEADERWIDGET_H

#include "dialogs/entryspeakerrordialog.h"
#include "logic/entry/entryphoneticoptions.h"
#include "logic/entry/entryspeaker.h"
#include "logic/sentence/sourcesentence.h"
#include "logic/settings/settingsutils.h"

#include <QEvent>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include <string>

// The EntryHeaderWidget displays basic information about the entry
// at the top of the detail view

class SentenceViewHeaderWidget : public QWidget
{
public:
    explicit SentenceViewHeaderWidget(QWidget *parent = nullptr);
    ~SentenceViewHeaderWidget() override;

    void changeEvent(QEvent *event) override;

    void setSourceSentence(const SourceSentence &sentence);

private:
    void setStyle(bool use_dark);
    void translateUI();

    void displaySentenceLabels(const EntryCharactersOptions options);
    void displayPronunciationLabels(const EntryPhoneticOptions options);
    void speakJyutping(void);
    void speakPinyin(void);

    void showError(const QString &reason, const QString &message);

    bool _paletteRecentlyChanged = false;

    QString _chinese;
    QString _jyutping;
    QString _pinyin;
    std::unique_ptr<EntrySpeaker> _speaker;

    QGridLayout *_sentenceHeaderLayout;
    QLabel *_sourceLanguageLabel;
    QLabel *_simplifiedLabel;
    QLabel *_traditionalLabel;
    QLabel *_jyutpingLabel;
    QPushButton *_jyutpingTTS;
    QLabel *_jyutpingPronunciation;
    QLabel *_pinyinLabel;
    QPushButton *_pinyinTTS;
    QLabel *_pinyinPronunciation;

    EntrySpeakErrorDialog *_message;
};

#endif // SENTENCEVIEWHEADERWIDGET_H

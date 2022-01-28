#ifndef SENTENCEVIEWHEADERWIDGET_H
#define SENTENCEVIEWHEADERWIDGET_H

#include "dialogs/entryspeakerrordialog.h"
#include "logic/entry/entrycharactersoptions.h"
#include "logic/entry/entryphoneticoptions.h"
#include "logic/entry/entryspeaker.h"
#include "logic/sentence/sourcesentence.h"

#include <QEvent>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include <memory>
#include <string>

// The SentenceViewHeaderWidget displays basic information about the sourceSentence
// at the top of the detail view

class SentenceViewHeaderWidget : public QWidget
{
public:
    explicit SentenceViewHeaderWidget(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

    void setSourceSentence(const SourceSentence &sentence);

private:
    void setupUI(void);
    void translateUI(void);
    void setStyle(bool use_dark);

    void displaySentenceLabels(const EntryCharactersOptions options);
    void displayPronunciationLabels(const EntryPhoneticOptions options);

    void showError(const QString &reason, const QString &message);

    bool _paletteRecentlyChanged = false;

    QString _chinese;
    QString _jyutping;
    QString _pinyin;
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    std::unique_ptr<EntrySpeaker> _speaker;
#endif

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

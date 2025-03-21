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
#include <QSettings>
#include <QWidget>

#include <memory>

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
    void displayPronunciationLabels(const SourceSentence &sentence,
                                    const CantoneseOptions &cantoneseOptions,
                                    const MandarinOptions &mandarinOptions);
    void clearPronunciationLabels(void);

    void showError(const QString &reason, const QString &message);

    bool _paletteRecentlyChanged = false;

    std::unique_ptr<QSettings> _settings;

    QString _chinese;
    QString _jyutping;
    QString _pinyin;
    std::unique_ptr<EntrySpeaker> _speaker;

    QGridLayout *_sentenceHeaderLayout;
    QLabel *_sourceLanguageLabel;
    QLabel *_simplifiedLabel;
    QLabel *_traditionalLabel;
    std::vector<QLabel *> _pronunciationTypeLabels;
    std::vector<QLabel *> _pronunciationLabels;

    QPushButton *_cantoneseTTS;
    bool _cantoneseTTSVisible = false;
    QPushButton *_mandarinTTS;
    bool _mandarinTTSVisible = false;

    EntrySpeakErrorDialog *_message;
};

#endif // SENTENCEVIEWHEADERWIDGET_H

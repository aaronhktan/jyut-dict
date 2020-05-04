#ifndef SENTENCECONTENTWIDGET_H
#define SENTENCECONTENTWIDGET_H

#include "logic/sentence/sentenceset.h"
#include "logic/sentence/sourcesentence.h"
#include "logic/entry/entrycharactersoptions.h"
#include "logic/entry/entryphoneticoptions.h"

#include <QEvent>
#include <QGridLayout>
#include <QLabel>
#include <QResizeEvent>
#include <QWidget>

#include <vector>

class SentenceContentWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SentenceContentWidget(QWidget *parent = nullptr);
    ~SentenceContentWidget() override;

    void changeEvent(QEvent *event) override;

    void setSentenceSet(const SentenceSet &set);
    void setSourceSentenceVector(
        const std::vector<SourceSentence> &sourceSentences);

private:
    void addLabelsToLayout(QGridLayout *layout,
                           int rowNumber,
                           QLabel *sentenceNumberLabel,
                           QLabel *simplifiedLabel,
                           QLabel *traditionalLabel,
                           QLabel *jyutpingLabel,
                           QLabel *pinyinLabel,
                           QLabel *sentenceLabel,
                           EntryPhoneticOptions phoneticOptions,
                           EntryCharactersOptions characterOptions);
    void setStyle(bool use_dark);

    void cleanupLabels();

    bool _paletteRecentlyChanged = false;

    QGridLayout *_sentenceLayout;
    std::vector<QLabel *> _sentenceNumberLabels;
    std::vector<QLabel *> _simplifiedLabels;
    std::vector<QLabel *> _traditionalLabels;
    std::vector<QLabel *> _jyutpingLabels;
    std::vector<QLabel *> _pinyinLabels;
    std::vector<QLabel *> _sentenceLabels;
};

#endif // SENTENCECONTENTWIDGET_H

#ifndef EXAMPLEVIEWHEADERWIDGET_H
#define EXAMPLEVIEWHEADERWIDGET_H

#include "logic/entry/entrycharactersoptions.h"
#include "logic/entry/entryphoneticoptions.h"

#include <QSettings>
#include <QWidget>

#include <memory>

class EntrySpeakErrorDialog;
class EntrySpeaker;
class Example;

class QEvent;
class QGridLayout;
class QLabel;
class QPushButton;

// The ExampleViewHeaderWidget displays basic information about the example
// at the top of the detail view

class ExampleViewHeaderWidget : public QWidget
{
public:
    explicit ExampleViewHeaderWidget(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

    void setExample(const Example &example);

private:
    void setupUI(void);
    void translateUI(void);
    void setStyle(bool use_dark);
    
    void displayExampleLabels(const EntryCharactersOptions options);
    void displayPronunciationLabels(const Example &example,
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

    QGridLayout *_exampleHeaderLayout;
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

#endif // EXAMPLEVIEWHEADERWIDGET_H

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
#include <vector>

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

    void displayPronunciationLabels(const Entry &entry,
                                    const CantoneseOptions &cantoneseOptions,
                                    const MandarinOptions &mandarinOptions);
    void clearPronunciationLabels(void);

    void showError(const QString &reason, const QString &message);

    bool _paletteRecentlyChanged = false;

    QString _chinese;
    QString _jyutping;
    QString _pinyin;
    std::unique_ptr<EntrySpeaker> _speaker;

    QGridLayout *_entryHeaderLayout;
    QLabel *_wordLabel;
    std::vector<QLabel *> _pronunciationTypeLabels;
    std::vector<QLabel *> _pronunciationLabels;

    QPushButton *_cantoneseTTS;
    bool _cantoneseTTSVisible = false;
    QPushButton *_mandarinTTS;
    bool _mandarinTTSVisible = false;

    EntrySpeakErrorDialog *_message;
};

#endif // ENTRYHEADERWIDGET_H

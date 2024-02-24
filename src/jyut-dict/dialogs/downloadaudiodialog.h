#ifndef DOWNLOADAUDIODIALOG_H
#define DOWNLOADAUDIODIALOG_H

#include "dialogs/defaultdialog.h"

// The DownloadAudioDialog prompts a user to download the audio
// files required for text-to-speech. On exec(), it will return
// QMessageBox::Yes if the user accepts.

class DownloadAudioDialog : public DefaultDialog
{
    Q_OBJECT
public:
    explicit DownloadAudioDialog(QWidget *parent = nullptr);

private:
    void setupUI(void);
};

#endif // RESETSETTINGSDIALOG_H

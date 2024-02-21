#ifndef DOWNLOADAUDIODIALOG_H
#define DOWNLOADAUDIODIALOG_H

#include "dialogs/defaultdialog.h"

class DownloadAudioDialog : public DefaultDialog
{
    Q_OBJECT
public:
    explicit DownloadAudioDialog(QWidget *parent = nullptr);

private:
    void setupUI(void);
};

#endif // RESETSETTINGSDIALOG_H

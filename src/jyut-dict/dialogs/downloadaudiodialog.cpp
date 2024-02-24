#include "downloadaudiodialog.h"

DownloadAudioDialog::DownloadAudioDialog(QWidget *parent)
    : DefaultDialog("", "", parent)
{
    setupUI();
    translateUI();
}

void DownloadAudioDialog::setupUI(void)
{
    setText(tr("To use this option for text-to-speech, files must be "
               "downloaded. Would you like to download them now?"));
    setInformativeText(
        tr("Downloading files requires a working Internet connection."));
    setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    setWidth(400);
}

#include "transcriptionerrordialog.h"

#include "logic/utils/utils.h"

#include <QAbstractButton>
#include <QDesktopServices>
#include <QPushButton>
#include <QUrl>

TranscriptionErrorDialog::TranscriptionErrorDialog(const QString &reason,
                                                   const QString &description,
                                                   QWidget *parent)
    : DefaultDialog(reason, description, parent)
{
    setupUI();
    translateUI();
}

void TranscriptionErrorDialog::setupUI()
{
    setText(tr("Dictation could not be started!"));

    addButton(tr("OK"), QMessageBox::AcceptRole);

#ifdef Q_OS_MAC
    QAbstractButton *microphonePerms = addButton(tr("Microphone access..."),
                                                 QMessageBox::HelpRole);
    disconnect(microphonePerms, nullptr, nullptr, nullptr);
    connect(microphonePerms, &QAbstractButton::clicked, this, [=, this]() {
        QDesktopServices::openUrl(QUrl{Utils::PRIVACY_MICROPHONE_LINK});
    });

    QAbstractButton *speechPerms = addButton(tr("Dictation access..."),
                                             QMessageBox::HelpRole);
    disconnect(speechPerms, nullptr, nullptr, nullptr);
    connect(speechPerms, &QAbstractButton::clicked, this, [=, this]() {
        QDesktopServices::openUrl(QUrl{Utils::PRIVACY_SPEECH_LINK});
    });
#endif

    setWidth(400);
    deselectButtons();
}

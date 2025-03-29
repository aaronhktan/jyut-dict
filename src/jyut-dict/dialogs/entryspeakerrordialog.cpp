#include "entryspeakerrordialog.h"

#include "logic/utils/utils.h"

#include <QAbstractButton>
#include <QDesktopServices>
#include <QPushButton>
#include <QUrl>

EntrySpeakErrorDialog::EntrySpeakErrorDialog(const QString &reason,
                                             const QString &description,
                                             QWidget *parent)
    : DefaultDialog(reason, description, parent)
{
    setupUI();
    translateUI();
}

void EntrySpeakErrorDialog::setupUI()
{
    setText(tr("Unable to speak entry!"));

    addButton(tr("OK"), QMessageBox::AcceptRole);

    QAbstractButton *addVoices = addButton(tr("Add voice"),
                                           QMessageBox::AcceptRole);
    disconnect(addVoices, nullptr, nullptr, nullptr);
    connect(addVoices, &QAbstractButton::clicked, this, [=, this]() {
        QDesktopServices::openUrl(QUrl{Utils::TTS_LINK});
    });

    QAbstractButton *helpVoices = addButton(tr("Help"), QMessageBox::HelpRole);
    disconnect(helpVoices, nullptr, nullptr, nullptr);
    connect(helpVoices, &QAbstractButton::clicked, this, [=, this]() {
        QDesktopServices::openUrl(QUrl{Utils::TTS_HELP_LINK});
    });

    setWidth(400);
    deselectButtons();
}

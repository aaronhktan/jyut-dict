#include "nosourceupdatesdialog.h"

#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined(Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif

NoSourceUpdatesDialog::NoSourceUpdatesDialog(QWidget *parent)
    : DefaultDialog{"", "", parent}
{
    setupUI();
    translateUI();
    setStyle(Utils::isDarkMode());
}

void NoSourceUpdatesDialog::setupUI()
{
    setText(tr("No dictionary updates found!"));
    setInformativeText(
        tr("All your dictionaries are on the most recent version."));
    setIcon(QMessageBox::Information);
    setStandardButtons(QMessageBox::Ok);
    setWidth(400);
}

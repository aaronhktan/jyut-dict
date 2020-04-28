#include "defaultdialog.h"

#include <QAbstractButton>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>

DefaultDialog::DefaultDialog(const QString &reason,
                             const QString &description,
                             QWidget *parent)
    : QMessageBox(parent)
{
    Qt::WindowFlags flags = windowFlags() | Qt::CustomizeWindowHint;
    flags &= ~(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint
               | Qt::WindowFullscreenButtonHint);
    setWindowFlags(flags);
    setInformativeText(reason);
    setDetailedText(description);
    setIcon(QMessageBox::Warning);
#ifdef Q_OS_WIN
    setWindowTitle(
        QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                    Strings::PRODUCT_NAME));
#elif defined(Q_OS_LINUX)
    setWindowTitle(" ");
#endif

    QList<QLabel *> labels = this->findChildren<QLabel *>();
    for (auto label : labels) {
        label->setTextInteractionFlags(Qt::NoTextInteraction);
    }

    // setDefaultButton doesn't really work, so use this
    // workaround to deselect all buttons first.
    for (auto button : buttons()) {
        QPushButton *b = static_cast<QPushButton *>(button);
        b->setDown(false);
        b->setAutoDefault(false);
        b->setDefault(false);
        b->setAttribute(Qt::WA_MacShowFocusRect, 0);
    }

    setWidth(400);
}

DefaultDialog::~DefaultDialog()
{
}

void DefaultDialog::setWidth(int width)
{
    // Setting minimum width also doesn't work, so use this
    // workaround to set a width.
    // Must be called after buttons are set!
    QSpacerItem *horizontalSpacer
        = new QSpacerItem(width,
                          0,
                          QSizePolicy::Minimum,
                          QSizePolicy::Minimum);
    QGridLayout *layout = static_cast<QGridLayout *>(this->layout());
    layout->addItem(horizontalSpacer,
                    layout->rowCount(),
                    0,
                    1,
                    layout->columnCount());
}

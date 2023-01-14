#ifndef TEXTTAB_H
#define TEXTTAB_H

#include "logic/settings/settingsutils.h"

#include <QEvent>
#include <QPushButton>
#include <QWidget>

class TextTab : public QWidget
{
    Q_OBJECT
public:
    explicit TextTab(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

private:
    void setupUI();
    void translateUI();
    void setStyle(bool use_dark);

    bool _paletteRecentlyChanged = false;

    std::unique_ptr<QSettings> _settings;
};

#endif // TEXTTAB_H

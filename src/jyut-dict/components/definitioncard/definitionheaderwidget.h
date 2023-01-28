#ifndef DEFINITIONHEADERWIDGET_H
#define DEFINITIONHEADERWIDGET_H

#include "components/definitioncard/icardheaderwidget.h"

#include <QEvent>
#include <QLabel>
#include <QSettings>
#include <QVBoxLayout>
#include <QWidget>

#include <string>

// The DefinitionHeaderWidget shows the source of definitions
// It contains a frame as a divider and the ability to set the source to display

class DefinitionHeaderWidget : public QWidget, public ICardHeaderWidget
{
public:
    explicit DefinitionHeaderWidget(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

    void setSectionTitle(const std::string &title) override;

private:
    void setStyle(bool use_dark);

    bool _paletteRecentlyChanged = false;

    std::unique_ptr<QSettings> _settings;

    QLabel *_titleLabel;
    QVBoxLayout *_layout;
};

#endif // DEFINITIONHEADERWIDGET_H

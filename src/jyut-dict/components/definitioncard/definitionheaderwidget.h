#ifndef DEFINITIONHEADERWIDGET_H
#define DEFINITIONHEADERWIDGET_H

#include "components/definitioncard/icardheaderwidget.h"

#include <QEvent>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

#include <string>

// The DefinitionHeaderWidget shows the source of definitions
// It contains a frame as a divider and the ability to set the source to display

class DefinitionHeaderWidget : public QWidget, public ICardHeaderWidget
{
public:
    explicit DefinitionHeaderWidget(QWidget *parent = nullptr);
    explicit DefinitionHeaderWidget(std::string title,
                                    QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

    void setSectionTitle(std::string title) override;

private:
    void setStyle(bool use_dark);

    bool _paletteRecentlyChanged = false;

    QLabel *_titleLabel;
    QVBoxLayout *_layout;
};

#endif // DEFINITIONHEADERWIDGET_H

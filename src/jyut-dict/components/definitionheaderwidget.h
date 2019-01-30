#ifndef DEFINITIONHEADERWIDGET_H
#define DEFINITIONHEADERWIDGET_H

#include <components/isectionheaderwidget.h>

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

// The DefinitionHeaderWidget shows the source of definitions
// It contains a frame as a divider and the ability to set the source to display

class DefinitionHeaderWidget : public QWidget, public ISectionHeaderWidget
{
public:
    explicit DefinitionHeaderWidget(QWidget *parent = nullptr);
    explicit DefinitionHeaderWidget(std::string title, QWidget *parent = nullptr);

    void setSectionTitle(std::string title) override;

    virtual ~DefinitionHeaderWidget() override;
private:
    QLabel *_titleLabel;
    QFrame *_divider;
    QVBoxLayout *_layout;

    std::string formatTitle(std::string title);
};

#endif // DEFINITIONHEADERWIDGET_H

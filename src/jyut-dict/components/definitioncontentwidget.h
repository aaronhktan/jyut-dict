#ifndef DEFINITIONCONTENTWIDGET_H
#define DEFINITIONCONTENTWIDGET_H

#include "logic/entry/entry.h"

#include <QEvent>
#include <QGridLayout>
#include <QLabel>
#include <QWidget>

#include <string>
#include <vector>

// The DefinitionContentWidget shows all the definitions
// It contains a collection of QLabels, one for each definition
// and its associated "number" (i.e. first definition, second definition, etc.)

class DefinitionContentWidget : public QWidget
{
public:
    explicit DefinitionContentWidget(QWidget *parent = nullptr);
    ~DefinitionContentWidget() override;

    void changeEvent(QEvent *event) override;

    void setEntry(const Entry &entry);
    void setEntry(std::vector<std::string> definitions);

private:
    void setStyle(bool use_dark);

    void cleanupLabels();

    bool _paletteRecentlyChanged = false;

    QGridLayout *_definitionLayout;
    std::vector<QLabel *> _definitionNumberLabels;
    std::vector<QLabel *> _definitionLabels;
};

#endif // DEFINITIONCONTENTWIDGET_H

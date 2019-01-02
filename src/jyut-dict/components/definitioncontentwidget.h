#ifndef DEFINITIONCONTENTWIDGET_H
#define DEFINITIONCONTENTWIDGET_H

#include "logic/entry/entry.h"

#include <QGridLayout>
#include <QLabel>
#include <QWidget>

class DefinitionContentWidget : public QWidget
{
public:
    explicit DefinitionContentWidget(QWidget *parent = nullptr);

    void setEntry(Entry& entry);
    void setEntry(std::vector<std::string> definitions);

private:
    QGridLayout *_definitionLayout;
    std::vector<QLabel *> _definitionNumberLabels;
    std::vector<QLabel *> _definitionLabels;
};

#endif // DEFINITIONCONTENTWIDGET_H

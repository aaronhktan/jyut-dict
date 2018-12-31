#ifndef DEFINITIONSCROLLAREA_H
#define DEFINITIONSCROLLAREA_H

#include "logic/entry/entry.h"

#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QResizeEvent>
#include <QScrollArea>
#include <QVBoxLayout>

class DefinitionScrollArea : public QScrollArea
{
public:
    explicit DefinitionScrollArea(QWidget *parent = nullptr);

    void resizeEvent(QResizeEvent *event) override;

    void setEntry(Entry& entry);

    ~DefinitionScrollArea() override;

private:
    // TODO: Separate these out into their own components
    // Entry Header
    QWidget *entryHeader;
    QGridLayout *entryHeaderLayout;
    QLabel *wordLabel;
    QLabel *jyutpingLabel;
    QLabel *jyutpingPronunciation;
    QLabel *pinyinLabel;
    QLabel *pinyinPronunciation;

    // Definition Header
    QWidget *definitionHeader;
    QVBoxLayout *definitionHeaderLayout;
    QLabel *definitionHeaderLabel;
    QFrame *separatorLine;

    // Definition
    QWidget *definition;
    QGridLayout *definitionLayout;
    std::vector<QLabel *>definitionNumberLabels;
    std::vector<QLabel *>definitionLabels;

    // Entire definition area (definition + header)
    QWidget *definitionArea;
    QVBoxLayout *definitionAreaLayout;

    // Entire Scroll Area
    QVBoxLayout *scrollAreaLayout;
    QWidget *scrollAreaWidget;
};

#endif // DEFINITIONSCROLLAREA_H

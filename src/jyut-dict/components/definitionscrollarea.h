#ifndef DEFINITIONSCROLLAREA_H
#define DEFINITIONSCROLLAREA_H

#include <QResizeEvent>
#include <QScrollArea>

class DefinitionScrollArea : public QScrollArea
{
public:
    explicit DefinitionScrollArea(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event) override;
    ~DefinitionScrollArea() override;

private:
    QWidget *scrollAreaWidget;
};

#endif // DEFINITIONSCROLLAREA_H

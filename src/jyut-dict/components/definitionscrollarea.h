#ifndef DEFINITIONSCROLLAREA_H
#define DEFINITIONSCROLLAREA_H

#include <QScrollArea>

class DefinitionScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit DefinitionScrollArea(QWidget *parent = nullptr);
    ~DefinitionScrollArea();

signals:

public slots:
};

#endif // DEFINITIONSCROLLAREA_H

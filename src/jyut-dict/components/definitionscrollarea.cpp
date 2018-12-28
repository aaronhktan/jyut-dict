#include "definitionscrollarea.h"

DefinitionScrollArea::DefinitionScrollArea(QWidget *parent) : QScrollArea(parent)
{
    this->setFrameShape(QFrame::NoFrame);
}

DefinitionScrollArea::~DefinitionScrollArea()
{

}

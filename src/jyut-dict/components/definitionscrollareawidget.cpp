#include "definitionscrollareawidget.h"

DefinitionScrollAreaWidget::DefinitionScrollAreaWidget(QWidget *parent)
    : QWidget(parent)
{
    // Entire Scroll Area
    _scrollAreaLayout = new QVBoxLayout{this};
    _scrollAreaLayout->setSpacing(25);
    _scrollAreaLayout->setContentsMargins(11, 11, 11, 11);

    setLayout(_scrollAreaLayout);

    _entryHeaderWidget = new EntryHeaderWidget{this};
    _definitionWidget = new DefinitionWidget{this};

    // Add all widgets to main layout
    _scrollAreaLayout->addWidget(_entryHeaderWidget);
    _scrollAreaLayout->addWidget(_definitionWidget);
    _scrollAreaLayout->addStretch(1);
    setStyleSheet("QWidget { background-color: #1E1E1E; }");
}

//QSize DefinitionScrollAreaWidget::sizeHint() const
//{
//    return QSize{width(),
//                 _entryHeaderWidget->sizeHint().height()
//                 +   _definitionWidget->sizeHint().height()};
//}

void DefinitionScrollAreaWidget::setEntry(const Entry &entry)
{
    _entryHeaderWidget->setEntry(entry);
    _definitionWidget->setEntry(entry);
//    updateGeometry();
}

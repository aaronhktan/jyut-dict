#include "definitioncontentwidget.h"

DefinitionContentWidget::DefinitionContentWidget(QWidget *parent) : QWidget(parent)
{
    _definitionLayout = new QGridLayout(this);
    _definitionLayout->setContentsMargins(0, 0, 0, 0);

    _definitionNumberLabels = {};
    _definitionLabels = {};

    setLayout(_definitionLayout);
}

void DefinitionContentWidget::setEntry(Entry& entry __unused)
{
//    TODO: Fix this
//    auto definitions = entry.getDefinitions();

//    setEntry(definitions);
}

void DefinitionContentWidget::setEntry(std::vector<std::string> definitions)
{
    for (size_t i = 0; i < definitions.size(); i++) {
        std::string number{"<font color=#6f6f6f>" + std::to_string(i + 1) + "</font>"};
        _definitionNumberLabels.push_back(new QLabel(number.c_str(), this));
        int definitionNumberWidth = _definitionNumberLabels.back()->fontMetrics().boundingRect("PY").width();
        _definitionNumberLabels.back()->setFixedWidth(definitionNumberWidth);

        _definitionLabels.push_back(new QLabel(definitions[i].c_str(), this));
        _definitionLabels.back()->setWordWrap(true);
        _definitionLabels.back()->setTextInteractionFlags(Qt::TextSelectableByMouse);

        _definitionLayout->addWidget(_definitionNumberLabels[i], static_cast<int>(i + 9), 0, Qt::AlignTop);
        _definitionLayout->addWidget(_definitionLabels[i], static_cast<int>(i + 9), 1, Qt::AlignTop);
    }
}

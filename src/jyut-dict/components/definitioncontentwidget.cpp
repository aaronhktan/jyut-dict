#include "definitioncontentwidget.h"

DefinitionContentWidget::DefinitionContentWidget(QWidget *parent) : QWidget(parent)
{
    _definitionLayout = new QGridLayout{this};
    _definitionLayout->setContentsMargins(10, 0, 10, 10);

    _definitionNumberLabels = {};
    _definitionLabels = {};

    setLayout(_definitionLayout);
}

DefinitionContentWidget::~DefinitionContentWidget()
{
    cleanupLabels();
}

//#include <QDebug>
//#include <QApplication>
//#include <QDesktopWidget>
//QSize DefinitionContentWidget::sizeHint() const
//{
//    int height = 20; // For top and bottom margin
//    for (size_t i = 0; i < _definitionLabels.size(); i++) {
////        QFontMetrics metrics(_definitionLabels[i]->font());
//int labelHeight = _definitionLabels[i]->sizeHint().height();
////        int labelHeight = _definitionLabels[i]->heightForWidth(width());
//        height += labelHeight + _definitionLayout->spacing();
////        qDebug() << "Label height:" << labelHeight;
//    }
////    qDebug() << "Total height of content widget: " << height;
//    return QSize(width(), height);
//}

void DefinitionContentWidget::setEntry(const Entry &entry)
{
    // Note: displays only the first set of definitions in entry
    if (entry.getDefinitionsSets().size() >= 1) {
        setEntry(entry.getDefinitionsSets()[0].getDefinitions());
    }
}

void DefinitionContentWidget::setEntry(std::vector<std::string> definitions)
{
    cleanupLabels();

    for (size_t i = 0; i < definitions.size(); i++) {
        std::string number = std::to_string(i + 1);
        _definitionNumberLabels.push_back(new QLabel{number.c_str(), this});
        _definitionNumberLabels.back()->setStyleSheet(
            "QLabel { color: #6F6F6F; }");
        int definitionNumberWidth = _definitionNumberLabels.back()
                                        ->fontMetrics()
                                        .boundingRect("PY")
                                        .width();
        _definitionNumberLabels.back()->setFixedWidth(definitionNumberWidth);
        int definitionNumberHeight = _definitionNumberLabels.back()
                                         ->fontMetrics()
                                         .boundingRect("PYing")
                                         .height();
        _definitionNumberLabels.back()->setFixedHeight(definitionNumberHeight);

        _definitionLabels.push_back(new /*ResizeableLabel*/ QLabel{this});
        _definitionLabels.back()->setText(definitions[i].c_str());
        _definitionLabels.back()->setWordWrap(true);
//        _definitionLabels.back()->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        _definitionLabels.back()->setTextInteractionFlags(Qt::TextSelectableByMouse);

        _definitionLayout->addWidget(_definitionNumberLabels[i],
                                     static_cast<int>(i + 9), 0, Qt::AlignTop);
        _definitionLayout->addWidget(_definitionLabels[i],
                                     static_cast<int>(i + 9), 1, Qt::AlignTop);
    }
}

//#include <QResizeEvent>
//void DefinitionContentWidget::resizeEvent(QResizeEvent *event)
//{
//    for (size_t i = 0; i < _definitionLabels.size(); i++) {
//        _definitionLabels[i]->setFixedWidth(event->size().width());
//    }
//    updateGeometry();
//}

void DefinitionContentWidget::cleanupLabels()
{
    for (auto label : _definitionNumberLabels) {
        delete label;
    }
    for (auto label : _definitionLabels) {
        delete label;
    }
}

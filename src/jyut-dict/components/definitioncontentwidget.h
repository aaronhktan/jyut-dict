#ifndef DEFINITIONCONTENTWIDGET_H
#define DEFINITIONCONTENTWIDGET_H

#include "components/resizeablelabel.h"
#include "logic/entry/entry.h"

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

//    QSize sizeHint() const override;

    void setEntry(const Entry &entry);
    void setEntry(std::vector<std::string> definitions);

private:
//    void resizeEvent(QResizeEvent *event) override;

    void cleanupLabels();

    QGridLayout *_definitionLayout;
    std::vector<QLabel *> _definitionNumberLabels;
    std::vector</*ResizeableLabel*/ QLabel *> _definitionLabels;
};

#endif // DEFINITIONCONTENTWIDGET_H

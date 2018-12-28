#include "searchlineedit.h"

#include <QIcon>

SearchLineEdit::SearchLineEdit(QWidget *parent) : QLineEdit(parent)
{
    this->setPlaceholderText("Search");

    QIcon icon = QIcon("/Users/aaron/Downloads/glass.png");
    QIcon clear = QIcon("/Users/aaron/Downloads/x.png");

    this->addAction(icon, QLineEdit::LeadingPosition);
    this->addAction(clear, QLineEdit::TrailingPosition);
    this->setStyleSheet("QLineEdit { \
                            border-radius: 3px; \
                            outline: 1px solid black; \
                            font-size: 12px; \
                            padding-top: 4px; \
                            padding-bottom: 4px; \
                            selection-background-color: darkgray; \
                            background-color: #bbbbbb; }");
}

SearchLineEdit::~SearchLineEdit()
{

}

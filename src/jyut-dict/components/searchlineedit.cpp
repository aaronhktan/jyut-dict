#include "searchlineedit.h"

#include <iostream>

#include <QIcon>

SearchLineEdit::SearchLineEdit(QWidget *parent) : QLineEdit(parent)
{
    this->setPlaceholderText("Search");

    QIcon icon = QIcon(":/search_inverted.png");
    QIcon clear = QIcon(":/x_inverted.png");

    _clearLineEdit = new QAction(clear, "", this);
    connect(_clearLineEdit, &QAction::triggered, [this](){this->clear();});

    this->addAction(icon, QLineEdit::LeadingPosition);
    this->addAction(_clearLineEdit, QLineEdit::TrailingPosition);
    this->setStyleSheet("QLineEdit { \
                            border-radius: 3px; \
                            outline: 1px solid black; \
                            font-size: 12px; \
                            padding-top: 4px; \
                            padding-bottom: 4px; \
                            selection-background-color: darkgray; \
                            background-color: #586365; }");
}

void SearchLineEdit::keyReleaseEvent(QKeyEvent *event)
{
    if (text().isEmpty()) {
        removeAction(_clearLineEdit);
    } else {
        addAction(_clearLineEdit, QLineEdit::TrailingPosition);
    }
    event->accept();
}

void SearchLineEdit::focusInEvent(QFocusEvent *event)
{
    if (text().isEmpty()) {
        removeAction(_clearLineEdit);
    } else {
        addAction(_clearLineEdit, QLineEdit::TrailingPosition);
    }
    QLineEdit::focusInEvent(event);
}

void SearchLineEdit::focusOutEvent(QFocusEvent *event)
{
    removeAction(_clearLineEdit);
    QLineEdit::focusOutEvent(event);
}

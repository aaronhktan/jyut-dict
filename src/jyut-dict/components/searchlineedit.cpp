#include "searchlineedit.h"

#include "logic/search/sqlsearch.h"

#include <QIcon>

#include <iostream>
#include <vector>

SearchLineEdit::SearchLineEdit(QWidget *parent) : QLineEdit(parent)
{
    setPlaceholderText("Search");

    QIcon icon = QIcon(":/images/search_inverted.png");
    QIcon clear = QIcon(":/images/x_inverted.png");

    _clearLineEdit = new QAction(clear, "", this);
    connect(_clearLineEdit, &QAction::triggered,
            [this](){
                this->clear();
                removeAction(_clearLineEdit);
                _search->searchEnglish(text());});

    addAction(icon, QLineEdit::LeadingPosition);
    addAction(_clearLineEdit, QLineEdit::TrailingPosition);
    setStyleSheet("QLineEdit { \
                     border-radius: 3px; \
                     border-color: black; \
                     border-width: 1px; \
                     font-size: 12px; \
                     padding-top: 4px; \
                     padding-bottom: 4px; \
                     selection-background-color: darkgray; \
                     background-color: #586365; }");

    _search = new SQLSearch();
}

void SearchLineEdit::keyReleaseEvent(QKeyEvent *event)
{
    if (text().isEmpty()) {
        removeAction(_clearLineEdit);
    } else {
        addAction(_clearLineEdit, QLineEdit::TrailingPosition);
    }
    event->accept();

    _search->searchEnglish(text());
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

SearchLineEdit::~SearchLineEdit()
{
    delete _clearLineEdit;
}

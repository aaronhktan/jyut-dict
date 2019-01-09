#include "searchlistwidget.h"

#include <QDebug>
#include <QFontMetrics>

SearchListWidget::SearchListWidget(QWidget *parent) : QWidget(parent)
{
    _widgetLayout = new QVBoxLayout(this);
    _widgetLayout->setSpacing(0);

    _wordLabel = new QLabel(this);
    _wordLabel->setStyleSheet("QLabel { font-size: 20px}");
    _pronunciationLabel = new QLabel(this);
    _definitionSnippetLabel = new QLabel(this);

    _widgetLayout->addWidget(_wordLabel);
    _widgetLayout->addWidget(_pronunciationLabel);
    _widgetLayout->addWidget(_definitionSnippetLabel);

    setLayout(_widgetLayout);
}

SearchListWidget::SearchListWidget(Entry& entry, QWidget *parent)
    : SearchListWidget(parent)
{
    _word = entry.getSimplified();
    _pronunciation = entry.getPinyin();
    _definitionSnippet = entry.getDefinitionSnippet();

    _wordLabel->setText(elideText(_word.c_str(), _wordLabel));
    _pronunciationLabel->setText(elideText(_pronunciation.c_str(), _pronunciationLabel));
    _definitionSnippetLabel->setText(elideText(_definitionSnippet.c_str(), _definitionSnippetLabel));
}

void SearchListWidget::resizeEvent(QResizeEvent *event)
{
    event->accept();

    _wordLabel->setText(elideText(_word.c_str(), _wordLabel));
    _pronunciationLabel->setText(elideText(_pronunciation.c_str(), _pronunciationLabel));
    _definitionSnippetLabel->setText(elideText(_definitionSnippet.c_str(), _definitionSnippetLabel));
}

QString SearchListWidget::elideText(QString text, QLabel *label)
{
    QFontMetrics metrics(label->font());
    QString elidedText = metrics.elidedText(text, Qt::ElideRight, label->width());
    return elidedText;
}

SearchListWidget::~SearchListWidget()
{
    delete _wordLabel;
    delete _pronunciationLabel;
    delete _definitionSnippetLabel;

    delete _widgetLayout;
}

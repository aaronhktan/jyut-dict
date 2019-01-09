#ifndef SEARCHLISTWIDGET_H
#define SEARCHLISTWIDGET_H

#include "logic/entry/entry.h"

#include <QLabel>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QWidget>

#include <string>

class SearchListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SearchListWidget(QWidget *parent = nullptr);
    explicit SearchListWidget(Entry& entry, QWidget *parent = nullptr);

    ~SearchListWidget() override;

private:
    void resizeEvent(QResizeEvent *event) override;

    QString elideText(QString text, QLabel *label);

    QVBoxLayout *_widgetLayout;

    QLabel *_wordLabel;
    QLabel *_pronunciationLabel;
    QLabel *_definitionSnippetLabel;

    std::string _word;
    std::string _pronunciation;
    std::string _definitionSnippet;

signals:

public slots:
};

#endif // SEARCHLISTWIDGET_H

#ifndef DICTIONARYTAB_H
#define DICTIONARYTAB_H

#include <QLabel>
#include <QLayout>
#include <QWidget>

class DictionaryTab : public QWidget
{
    Q_OBJECT

public:
    explicit DictionaryTab(QWidget *parent = nullptr);
    explicit DictionaryTab(QWidget *parent = nullptr, QString text="hi");

private:
    QLabel *_title;

    QLayout *_tabLayout;

signals:

public slots:
};

#endif // DICTIONARYTAB_H

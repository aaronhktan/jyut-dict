#ifndef DICTIONARYLISTMODEL_H
#define DICTIONARYLISTMODEL_H


#include <QWidget>

// The DictionaryListModel contains a list of all currently installed
// dictionaries.

class DictionaryListModel : public QWidget
{
    Q_OBJECT
public:
    explicit DictionaryListModel(QWidget *parent = nullptr);

    void setDictionaries();
signals:

public slots:
};

#endif // DICTIONARYLISTMODEL_H

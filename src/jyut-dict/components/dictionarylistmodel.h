#ifndef DICTIONARYLISTMODEL_H
#define DICTIONARYLISTMODEL_H

#include "logic/entry/dictionarysource.h"

#include <QAbstractListModel>

#include <vector>

// The DictionaryListModel contains a list of all currently installed
// dictionaries.

class DictionaryListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit DictionaryListModel(QObject *parent = nullptr);

    void setDictionaries(std::vector<DictionarySource> dictionaries);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

private:
    std::vector<DictionarySource> _dictionaries;

signals:

public slots:
};

#endif // DICTIONARYLISTMODEL_H

#ifndef SOURCELISTMODEL_H
#define SOURCELISTMODEL_H

#include "logic/source/sourcemetadata.h"

#include <QAbstractListModel>
#include <QModelIndex>
#include <QObject>
#include <QVariant>

#include <vector>

// The SourceListModel contains a list of all currently installed
// sources.

class SourceListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit SourceListModel(QObject *parent = nullptr);

    void setDictionaries(std::vector<SourceMetadata> dictionaries);
    bool setData(const QModelIndex &index,
                 const QVariant &value,
                 int role = Qt::EditRole) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

private:
    std::vector<SourceMetadata> _dictionaries;
};

#endif // SOURCELISTMODEL_H

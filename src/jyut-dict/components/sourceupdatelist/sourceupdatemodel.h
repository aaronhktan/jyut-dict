#ifndef SOURCEUPDATEMODEL_H
#define SOURCEUPDATEMODEL_H

#include "logic/dictionary/dictionarymetadata.h"
#include "logic/update/iupdatechecker.h"

#include <QAbstractTableModel>

#include <vector>

class SourceUpdateModel : public QAbstractTableModel
{
public:
    struct MetadataWrapper
    {
        DictionaryMetadata current;
        IUpdateChecker::SourceManifestMetadata available;
        bool checked = true;
    };

    enum Columns {
        kNameColumn = 0,
        kInstalledVersionColumn = 1,
        kNewVersionColumn = 2,
        kCheckColumn = 3,
        kNumColumns,
    };

    enum UserRoles {
        kUpdateInfo = Qt::UserRole,
    };

    explicit SourceUpdateModel(std::vector<MetadataWrapper> &w,
                               QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index,
                 const QVariant &value,
                 int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role) const override;

private:
    std::vector<MetadataWrapper> _metadata;
};

#endif // SOURCEUPDATEMODEL_H

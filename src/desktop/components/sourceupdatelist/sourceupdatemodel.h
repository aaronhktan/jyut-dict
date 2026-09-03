#ifndef SOURCEUPDATEMODEL_H
#define SOURCEUPDATEMODEL_H

#include "logic/source/sourcemetadata.h"
#include "logic/update/iupdatechecker.h"

#include <QAbstractTableModel>

#include <vector>

// The SourceUpdateModel is the backing model for metadata about sources
// that can be updated.

class SourceUpdateModel : public QAbstractTableModel
{
    // This is required in order for the headers in the table to be
    // properly translated. In order for tr() to work, Qt must have
    // a "context" for the translated string, and Q_OBJECT allows Qt
    // to generate that context.
    // https://doc.qt.io/qt-6/qtranslator.html#looking-up-translations
    Q_OBJECT

public:
    struct MetadataWrapper
    {
        SourceMetadata installedMetadata;
        IUpdateChecker::SourceManifestMetadata availableMetadata;
        bool checked = true;
    };

    enum Columns {
        kNameColumn = 0,
        kInstalledVersionColumn = 1,
        kAvailableVersionColumn = 2,
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

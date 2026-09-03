#include "sourceupdatemodel.h"

#include "logic/utils/utils_qt.h"

#include <QApplication>
#include <QGuiApplication>
#include <QPalette>

SourceUpdateModel::SourceUpdateModel(std::vector<MetadataWrapper> &w,
                                     QObject *parent)
    : QAbstractTableModel{parent}
    , _metadata{w}
{}

int SourceUpdateModel::rowCount(const QModelIndex &parent) const
{
    // This is a table, not a tree, so the rowCount of anything with a parent is 0.
    if (parent.isValid()) {
        return 0;
    }

    // If the parent is invalid, then we are at the root, so we return the number of rows in the table.
    return _metadata.size();
}

int SourceUpdateModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return kNumColumns;
}

QVariant SourceUpdateModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= _metadata.size()) {
        return {};
    }

    const MetadataWrapper &metadata = _metadata.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case kNameColumn: {
            return QString::fromStdString(metadata.installedMetadata.getName());
        }
        case kInstalledVersionColumn: {
            return QString::fromStdString(
                metadata.installedMetadata.getVersion());
        }
        case kAvailableVersionColumn: {
            return QString::fromStdString(
                metadata.availableMetadata.versionNumber);
        }
        default: {
            return {};
        }
        }
    }

    if (role == Qt::CheckStateRole && index.column() == kCheckColumn) {
        return metadata.checked ? Qt::Checked : Qt::Unchecked;
    }

    if (metadata.checked && role == Qt::BackgroundRole) {
        if (QGuiApplication::applicationState() == Qt::ApplicationInactive) {
            return QApplication::palette().brush(QPalette::Inactive,
                                                 QPalette::Highlight);
        } else {
            return QApplication::palette().brush(QPalette::Active,
                                                 QPalette::Highlight);
        }
    }

    if (metadata.checked && role == Qt::ForegroundRole) {
        QColor backgroundColour;
        if (QGuiApplication::applicationState() == Qt::ApplicationInactive) {
            backgroundColour = QApplication::palette()
                                   .brush(QPalette::Inactive,
                                          QPalette::Highlight)
                                   .color();
        } else {
            backgroundColour = QApplication::palette()
                                   .brush(QPalette::Active, QPalette::Highlight)
                                   .color();
        }
        return QBrush{Utils::getContrastingColour(backgroundColour)};
    }

    if (role == Qt::TextAlignmentRole && index.column() == kCheckColumn) {
        return Qt::AlignCenter;
    }

    if (role == SourceUpdateModel::UserRoles::kUpdateInfo) {
        return QVariant::fromValue(&metadata.availableMetadata);
    }

    return {};
}

bool SourceUpdateModel::setData(const QModelIndex &index,
                                const QVariant &value,
                                int role)
{
    // Currently only handles setting data for whether something was checked or unchecked
    if (!index.isValid() || index.column() != kCheckColumn
        || role != Qt::CheckStateRole) {
        return false;
    }

    const bool checked = value.toInt() == Qt::Checked;
    MetadataWrapper &metadata = _metadata.at(index.row());

    if (metadata.checked == checked) {
        return true;
    }

    metadata.checked = checked;
    emit dataChanged(this->index(index.row(), 0),
                     this->index(index.row(), kNumColumns - 1),
                     {Qt::CheckStateRole,
                      Qt::BackgroundRole,
                      Qt::ForegroundRole});
    return true;
}

Qt::ItemFlags SourceUpdateModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsEnabled;
}

QVariant SourceUpdateModel::headerData(int section,
                                       Qt::Orientation orientation,
                                       int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return {};
    }

    switch (section) {
    case kNameColumn:
        return tr("Name");
    case kInstalledVersionColumn:
        return tr("Installed");
    case kAvailableVersionColumn:
        return tr("Available");
    case kCheckColumn:
        return tr("Update?");
    default:
        return {};
    }
}

#ifndef SOURCERELEASECHECKER_H
#define SOURCERELEASECHECKER_H

#include "logic/database/sqldatabaseutils.h"
#include "logic/source/sourcemetadata.h"
#include "logic/update/iupdatechecker.h"

#include <QObject>

#include <unordered_map>

class SQLDatabaseManager;

class QNetworkAccessManager;
class QNetworkReply;

// SourceReleaseChecker downloads one or more source manifests, and then
// checks each downloaded manifest for available updates. These are decided
// on the basis of the installed version vs available version declared in
// the manifest.

class SourceReleaseChecker : public QObject, public IUpdateChecker
{
    Q_OBJECT

public:
    explicit SourceReleaseChecker(std::shared_ptr<SQLDatabaseManager> manager,
                                  QObject *parent = nullptr);

    void checkForNewUpdate(void) override;

private:
    bool parseJSON(
        const std::string &data,
        std::unordered_map<std::string, IUpdateChecker::SourceManifestMetadata>
            &availability);

    QNetworkAccessManager *_networkManager;
    std::shared_ptr<SQLDatabaseManager> _databaseManager;
    std::unique_ptr<SQLDatabaseUtils> _utils;

    std::unordered_set<QNetworkReply *> _replies;

    std::unordered_map<std::string, SourceMetadata> _sourceMetadata;
    std::unordered_set<std::string> _sourceUpdateURLs;
    std::vector<SourceManifestMetadata> _updates;

public slots:
    void parseReply(QNetworkReply *request);

signals:
    void foundUpdate(const IUpdateChecker::UpdateVariant &v) override;
};

#endif // SOURCERELEASECHECKER_H

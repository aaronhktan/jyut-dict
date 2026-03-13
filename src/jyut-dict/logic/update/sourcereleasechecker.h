#ifndef SOURCERELEASECHECKER_H
#define SOURCERELEASECHECKER_H

#include "logic/database/sqldatabasemanager.h"
#include "logic/database/sqldatabaseutils.h"
#include "logic/dictionary/dictionarymetadata.h"
#include "logic/update/iupdatechecker.h"

#include <QObject>
#include <QtNetwork>

#include <unordered_map>

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
        std::unordered_map<std::string, IUpdateChecker::SourceUpdateAvailability>
            &availability);

    QNetworkAccessManager *_networkManager;
    std::shared_ptr<SQLDatabaseManager> _databaseManager;
    std::unique_ptr<SQLDatabaseUtils> _utils;

    std::unordered_set<QNetworkReply *> _replies;

    std::unordered_map<std::string, DictionaryMetadata> _sourceMetadata;
    std::unordered_set<std::string> _sourceUpdateURLs;
    std::vector<SourceUpdateAvailability> _updates;

public slots:
    void parseReply(QNetworkReply *request);

signals:
    void foundUpdate(const IUpdateChecker::UpdateVariant &v) override;
};

#endif // SOURCERELEASECHECKER_H

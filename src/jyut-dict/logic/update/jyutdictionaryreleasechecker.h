#ifndef JYUTDICTIONARYRELEASECHECKER_H
#define JYUTDICTIONARYRELEASECHECKER_H

#include "logic/update/iupdatechecker.h"

#include <QObject>
#include <QtNetwork>

#include <string>

// The JyutDictionaryReleaseChecker checks the Jyut Dictionary website
// for updates to the application

class JyutDictionaryReleaseChecker : public QObject, public IUpdateChecker
{
    Q_OBJECT
public:
    explicit JyutDictionaryReleaseChecker(QObject *parent);

    void checkForNewUpdate(void) override;

private:
    void preConnectToHost(void) const;

    bool parseJSON(const std::string &data,
                   bool &updateAvailable,
                   std::string &versionNumber,
                   std::string &url, std::string &description) const;

    QNetworkAccessManager *_manager;
    QNetworkReply *_reply;

public slots:
    void parseReply(QNetworkReply *request);

signals:
    void foundUpdate(bool updateAvailable,
                     std::string versionNumber,
                     std::string url,
                     std::string description) override;
};

#endif // JYUTDICTIONARYRELEASECHECKER_H

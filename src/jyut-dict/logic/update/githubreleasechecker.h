#ifndef GITHUBRELEASECHECKER_H
#define GITHUBRELEASECHECKER_H

#include "logic/update/iupdatechecker.h"

#include <QObject>
#include <QtNetwork>

#include <string>

// The GithubReleaseChecker checks the Github API for updates to the application

constexpr auto GITHUB_UPDATE_URL = "https://api.github.com/repos/aaronhktan/jyut-dict/releases";

class GithubReleaseChecker : public QObject, public IUpdateChecker
{
    Q_OBJECT
public:
    explicit GithubReleaseChecker(QObject *parent);

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
    void parseReply(QNetworkReply *request) const;

signals:
    void foundUpdate(bool updateAvailable,
                     std::string versionNumber,
                     std::string url, std::string description) const;
};

#endif // GITHUBRELEASECHECKER_H

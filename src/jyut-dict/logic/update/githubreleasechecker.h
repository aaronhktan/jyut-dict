#ifndef GITHUBRELEASECHECKER_H
#define GITHUBRELEASECHECKER_H

#include "logic/update/iupdatechecker.h"

#include <QObject>
#include <QtNetwork>

#include <string>

constexpr auto GITHUB_UPDATE_URL = "https://api.github.com/repos/aaronhktan/jyut-dict/releases";

class GithubReleaseChecker : public QObject, public IUpdateChecker
{
    Q_OBJECT
public:
    explicit GithubReleaseChecker(QObject *parent);
    ~GithubReleaseChecker() override;

    void checkForNewUpdate(void) override;

private:
    void preConnectToHost(void);

    bool parseJSON(const std::string &data,
                   bool &updateAvailable,
                   std::string &versionNumber,
                   std::string &url, std::string &description);
    void notifyNewUpdate(void);

    QNetworkAccessManager *_manager;
    QNetworkReply *_reply;

public slots:
    void parseReply(QNetworkReply *request);

signals:
    void foundUpdate(bool updateAvailable,
                     std::string versionNumber,
                     std::string url, std::string description);
};

#endif // GITHUBRELEASECHECKER_H

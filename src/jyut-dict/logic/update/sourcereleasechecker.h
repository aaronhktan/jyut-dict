#ifndef SOURCERELEASECHECKER_H
#define SOURCERELEASECHECKER_H

#include "logic/update/iupdatechecker.h"

#include <QObject>
#include <QtNetwork>

class SourceReleaseChecker : public QObject, public IUpdateChecker
{
    Q_OBJECT
public:
    explicit SourceReleaseChecker(QObject *parent = nullptr);

    void checkForNewUpdate(void) override;

private:
    bool parseResponse(const std::string &data);

    QNetworkAccessManager *_manager;
    QNetworkReply *_reply;

public slots:
    void parseReply(QNetworkReply *request);

signals:
    void foundUpdate(const IUpdateChecker::UpdateVariant &v) override;
};

#endif // SOURCERELEASECHECKER_H

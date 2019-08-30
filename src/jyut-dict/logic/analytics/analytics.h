#ifndef ANALYTICS_H
#define ANALYTICS_H

#include "logic/analytics/analyticsconfig.h"

#include <QtNetwork>
#include <QObject>
#include <QUrl>

#include <string>

// The Analytics class sends basic information to Google Analytics
// using the event API.

constexpr auto ANALYTICS_URL = "https://www.google-analytics.com/collect";

class Analytics : public QObject
{
Q_OBJECT

public:
    Analytics(QObject *parent = nullptr);

    void startSession();
    void endSession();

    void sendEvent(std::string category,
                   std::string action,
                   std::string label = "",
                   std::string value = "");

//    void sendPageview(std::string title, std::string url);

    void sendScreenview(std::string name);

private:
    void preConnectToHost();
    void sendNetworkRequest(QUrlQuery query);
    void gotReply();

    QString getUserAgent();

    QNetworkAccessManager *_manager;
    QNetworkReply *_reply;
};

#endif // ANALYTICS_H

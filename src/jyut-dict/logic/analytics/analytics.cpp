#include "analytics.h"

#include "logic/settings/settingsutils.h"
#include "logic/strings/strings.h"
#include "logic/utils/utils.h"

#include <QApplication>
#include <QLocale>
#include <QNetworkAccessManager>
#include <QSysInfo>
#include <QUrlQuery>
#include <QUuid>

Analytics::Analytics(QObject *parent)
    : QObject{parent}
{
    _manager = new QNetworkAccessManager{this};

    // Force SSL initialization to prevent GUI freeze
    // If not done, first GET will lazily many OpenSSL
    // libraries, causing a significant slowdown.
    // https://bugreports.qt.io/browse/QTBUG-61497

    // If this gives you an error, make sure to install
    // the appropriate OpenSSL for your operating system
    // (version <1.1.0, e.g. 1.0.2, since 1.1.0 breaks Qt)
    // and then copy the libeay.dll and ssleay.dll
    // files to the Qt compiler bin directories.
    //
    // This primarily affects Windows devices.
    QSslSocket::supportsSsl();

    // QNetworkAccessManager does a lazy load of a library the first time
    // it connects to a host, leading to the first GET feeling slow.
    // By connecting to a host first, we avoid this problem.
    //
    // Although, we wait for a few milliseconds before doing this
    // so that it does not block the GUI of the app starting up.
    //
    // This primarily affects macOS devices.
    QTimer::singleShot(100, this, &Analytics::preConnectToHost);
}

void Analytics::startSession()
{
    QUrlQuery query;
    query.addQueryItem("sc", "start");
    sendNetworkRequest(query);
}

void Analytics::endSession()
{
    QUrlQuery query;
    query.addQueryItem("sc", "end");
    sendNetworkRequest(query);
}

void Analytics::sendEvent(std::string category,
                          std::string action,
                          std::string label,
                          std::string value)
{
    QUrlQuery query;
    query.addQueryItem("t", "event");
    query.addQueryItem("ec", category.c_str());
    query.addQueryItem("ea", action.c_str());
    if (!label.empty()) {
        query.addQueryItem("el", label.c_str());
    }
    if (!value.empty()) {
        query.addQueryItem("ev", value.c_str());
    }
    sendNetworkRequest(query);
}

//void Analytics::sendPageview(std::string title, std::string url)
//{
//    QUrlQuery query;
//    query.addQueryItem("t", "pageview");
//    query.addQueryItem("dl", url);
//    query.addQueryItem("dt", title.c_str());
//    sendNetworkRequest(query);
//}

void Analytics::sendScreenview(std::string name)
{
    QUrlQuery query;
    query.addQueryItem("t", "screenview");
    query.addQueryItem("cd", name.c_str());
    sendNetworkRequest(query);
}

void Analytics::preConnectToHost()
{
    _manager->connectToHostEncrypted(QUrl{ANALYTICS_URL}.host());
}

void Analytics::sendNetworkRequest(QUrlQuery query)
{
    QNetworkRequest request = QNetworkRequest{QUrl{ANALYTICS_URL}};
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::UserAgentHeader, getUserAgent());

    QString uuid = Settings::getSettings()
                       ->value("Analytics/uuid",
                               QVariant{QUuid::createUuid().toString()})
                       .toString();

    query.addQueryItem("v", "1"); // Version
    query.addQueryItem("tid", PROPERTY_ID); // Property ID
    query.addQueryItem("cid", uuid); // User ID
    query.addQueryItem("an", Strings::PRODUCT_NAME); // Application name
    query.addQueryItem("av", Utils::CURRENT_VERSION); // Application version
    query.addQueryItem("aip", "1"); // Enable IP anonymization
    query.addQueryItem("ds", "desktop"); // Set data source to desktop
    query.addQueryItem("ul", Settings::getCurrentLocale().name()); // Set user language

    QByteArray body;
    body.append(query.query());
    _reply = _manager->post(request, body);
    connect(_reply, &QNetworkReply::finished, this, &Analytics::gotReply);
}

void Analytics::gotReply()
{
//    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
//    qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
}

QString Analytics::getUserAgent()
{
    QString locale = Settings::getCurrentLocale().name();
    QString system = QSysInfo::prettyProductName();

    return QString{"%1/%2 (%3;%4) Analytics/1.0 (Qt/%5)"}
        .arg(Strings::PRODUCT_NAME)
        .arg(Utils::CURRENT_VERSION)
        .arg(system)
        .arg(locale)
        .arg(QT_VERSION_STR);
}

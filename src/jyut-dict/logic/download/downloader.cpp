#include "downloader.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

#include <cerrno>
#include <thread>

Downloader::Downloader(QUrl url, QString outputPath, QObject *parent)
    : QObject{parent}
    , _outputPath{outputPath}
{
    std::this_thread::sleep_for(std::chrono::seconds(5));

    _manager = new QNetworkAccessManager;
    _manager->setRedirectPolicy(QNetworkRequest::UserVerifiedRedirectPolicy);
    connect(_manager,
            &QNetworkAccessManager::finished,
            this,
            &Downloader::fileDownloaded);

    QNetworkRequest request{url};

    _reply = _manager->get(request);
    connect(_reply,
            &QNetworkReply::errorOccurred,
            this,
            &Downloader::errorOccurred);
    // Allow all redirects (insecure, but leave it for now)
    connect(_reply, &QNetworkReply::redirected, this, [&]() {
        emit _reply->redirectAllowed();
    });
}

void Downloader::fileDownloaded(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        emit error(ENETDOWN);
        return;
    }
    _downloadedData = reply->readAll();

    std::this_thread::sleep_for(std::chrono::seconds(5));

    reply->deleteLater();

    QFile file{_outputPath};
    if (file.exists()) {
        file.remove();
    }
    if (!file.open(QIODevice::ReadWrite)) {
        qDebug() << "Couldn't open file" << _outputPath;
        emit error(EPERM);
        return;
    }
    int numBytes = _downloadedData.size();
    if (file.write(_downloadedData) != numBytes) {
        qDebug() << "Couldn't write to file";
        emit error(ENOSPC);
        return;
    }
    file.close();

    emit downloaded(_outputPath);
}

void Downloader::errorOccurred([[maybe_unused]] QNetworkReply::NetworkError code)
{
    emit error(ENETDOWN);
}

QByteArray Downloader::downloadedData() const
{
    return _downloadedData;
}

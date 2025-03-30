#include "downloader.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QtConcurrent/QtConcurrent>

#include <cerrno>

Downloader::Downloader(QUrl url,
                       QString outputPath,
                       QObject *parent)
    : QObject{parent}
    , _url{url}
    , _outputPath{outputPath}
{}

void Downloader::startDownload()
{
    std::ignore = QtConcurrent::run([&]() {
        QEventLoop loop;

        _manager = new QNetworkAccessManager;
        connect(_manager,
                &QNetworkAccessManager::finished,
                this,
                &Downloader::fileDownloaded);

        QNetworkRequest request{_url};
        _reply = _manager->get(request);

        connect(_reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

        loop.exec();
        return;
    });
}

void Downloader::fileDownloaded(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        emit error(ENETDOWN);
        return;
    }
    _downloadedData = reply->readAll();

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

QByteArray Downloader::downloadedData() const
{
    return _downloadedData;
}

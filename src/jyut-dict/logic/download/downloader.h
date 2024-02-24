#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>

#include <any>

// The Downloader class provides a simple interface to download a file
// from a URL to a specified location on disk in a background thread.
// This class is NOT thread-safe!

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QUrl url,
                        QString outputPath,
                        std::any callbacks,
                        QObject *parent = nullptr);
    void startDownload();
    QByteArray downloadedData() const;

signals:
    void downloaded(QString outputPath, std::any callbacks);
    void error(int err);

private slots:
    void fileDownloaded(QNetworkReply *reply);

private:
    QNetworkAccessManager *_manager;
    QNetworkRequest *_request;
    QNetworkReply *_reply;
    QByteArray _downloadedData;
    QUrl _url;
    QString _outputPath;
    std::any _callbacks;
};

#endif // DOWNLOADER_H

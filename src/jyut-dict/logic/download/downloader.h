#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>

// The Downloader class provides a simple interface to download a file
// from a URL to a specified location on disk.

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QUrl url, QString outputPath, QObject *parent = 0);
    QByteArray downloadedData() const;

signals:
    void downloaded(QString outputPath);
    void error(int err);

private slots:
    void errorOccurred(QNetworkReply::NetworkError code);
    void fileDownloaded(QNetworkReply *reply);

private:
    QNetworkAccessManager *_manager;
    QNetworkReply *_reply;
    QByteArray _downloadedData;
    QString _outputPath;
};

#endif // DOWNLOADER_H

#include "sourcereleasechecker.h"

#include "logic/database/sqldatabasemanager.h"
#include "logic/utils/utils.h"

#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QtNetwork>

#include <iostream>

namespace {
constexpr auto DEFAULT_SOURCE_UPDATE_URL
    = "https://jyutdictionary.com/static/updates/v1/sources_manifest.json";

constexpr auto VERSION_NUMBER_COMPONENTS_SIZE = 3;
} // namespace

SourceReleaseChecker::SourceReleaseChecker(
    std::shared_ptr<SQLDatabaseManager> manager, QObject *parent)
    : QObject{parent}
    , _databaseManager{manager}
    , _utils{new SQLDatabaseUtils}
    , _networkManager{new QNetworkAccessManager{this}}
{
    QSslSocket::supportsSsl();
}

void SourceReleaseChecker::checkForNewUpdate()
{
    _updates.clear();
    std::vector<SourceMetadata> sources;
    QSqlDatabase db = _databaseManager->getDatabase();
    _utils->readSources(db, sources);

    for (const auto &s : sources) {
        _sourceMetadata[s.getName()] = s;
        _sourceUpdateURLs.emplace(s.getUpdateURL().empty()
                                      ? DEFAULT_SOURCE_UPDATE_URL
                                      : s.getUpdateURL());
    }

    if (_sourceUpdateURLs.empty()) {
        emit foundUpdate(_updates);
        return;
    }

    for (const auto &u : _sourceUpdateURLs) {
        QNetworkRequest request{QUrl{u.c_str()}};
        QNetworkReply *reply = _networkManager->get(request);
        _replies.emplace(reply);
        connect(reply, &QNetworkReply::finished, this, [this, u, reply]() {
            if (reply->error() == QNetworkReply::NoError) {
                parseReply(reply);
            } else {
                std::cerr << "Network request failed with error "
                          << reply->error() << std::endl;
            }
            _sourceUpdateURLs.erase(u);
            if (_sourceUpdateURLs.empty()) {
                emit foundUpdate(_updates);
            }

            disconnect(reply, nullptr, nullptr, nullptr);
            reply->deleteLater();
            _replies.erase(reply);
        });

        // Time out after 15 seconds
        QTimer::singleShot(15000, reply, [this, u, reply]() {
            _sourceUpdateURLs.erase(u);
            if (_sourceUpdateURLs.empty()) {
                emit foundUpdate(_updates);
            }

            disconnect(reply, nullptr, nullptr, nullptr);
            reply->abort();
            reply->deleteLater();
            _replies.erase(reply);
        });
    }
}

bool SourceReleaseChecker::parseJSON(
    const std::string &data,
    std::unordered_map<std::string, IUpdateChecker::SourceManifestMetadata>
        &availability)
{
    QJsonDocument doc = QJsonDocument::fromJson(
        QString::fromStdString(data.c_str()).toUtf8());
    QJsonValue source;
    for (const auto &source : doc.array()) {
        QJsonObject sourceObject = source.toObject();

        // Check if the user_version is supported
        if (!sourceObject.contains("userVersion")) {
            std::cerr
                << "Fetched object does not contain userVersion field, skipping"
                << std::endl;
            continue;
        }
        int userVersion{sourceObject.value("userVersion").toInt()};
        if (userVersion > CURRENT_DATABASE_VERSION) {
            continue;
        }

        // Check if sourceName corresponds to an installed dictionary
        if (!sourceObject.contains("sourceName")) {
            std::cerr
                << "Fetched object does not contain sourceName field, skipping"
                << std::endl;
            continue;
        }
        std::string sourceName
            = sourceObject.value("sourceName").toString().toStdString();
        if (!_sourceMetadata.contains(sourceName)) {
            continue;
        }

        // Parse source's version number
        std::vector<std::string> sourceVersionNumberComponents;
        Utils::split(_sourceMetadata.at(sourceName).getVersion(),
                     "-",
                     sourceVersionNumberComponents);
        if (sourceVersionNumberComponents.size()
            != VERSION_NUMBER_COMPONENTS_SIZE) {
            std::cerr << "Expected current versionNumber to have "
                      << VERSION_NUMBER_COMPONENTS_SIZE << ", got "
                      << sourceVersionNumberComponents.size();
            continue;
        }
        auto [year, month, day] = std::array<int, 3>(
            {std::stoi(sourceVersionNumberComponents[0]),
             std::stoi(sourceVersionNumberComponents[1]),
             std::stoi(sourceVersionNumberComponents[2])});

        // Check if version on the web is larger than local version
        if (!sourceObject.contains("versionNumber")) {
            std::cerr << "Fetched object does not contain versionNumber field, "
                         "skipping"
                      << std::endl;
            continue;
        }
        bool webVersionIsGreater = false;
        std::string webVersionNumber{
            sourceObject.value("versionNumber").toString().toStdString()};
        std::vector<std::string> webVersionNumberComponents;
        Utils::split(webVersionNumber, "-", webVersionNumberComponents);
        if (webVersionNumberComponents.size()
            != VERSION_NUMBER_COMPONENTS_SIZE) {
            std::cerr << "Expected web versionNumber to have "
                      << VERSION_NUMBER_COMPONENTS_SIZE << ", got "
                      << webVersionNumberComponents.size();
            continue;
        }
        auto [webYear, webMonth, webDay] = std::array<int, 3>(
            {std::stoi(webVersionNumberComponents[0]),
             std::stoi(webVersionNumberComponents[1]),
             std::stoi(webVersionNumberComponents[2])});
        if (webYear > year) {
            webVersionIsGreater = true;
        } else if (webYear == year && webMonth > month) {
            webVersionIsGreater = true;
        } else if (webYear == year && webMonth == month && webDay > day) {
            webVersionIsGreater = true;
        } else {
            webVersionIsGreater = false;
        }

        if (!webVersionIsGreater) {
            continue;
        }

        // Also check that the web version is greater than any version we already parsed
        if (availability.contains(sourceName)) {
            std::vector<std::string> parsedVersionNumberComponents;
            Utils::split(availability.at(sourceName).versionNumber,
                         "-",
                         parsedVersionNumberComponents);
            auto [parsedYear, parsedMonth, parsedDay] = std::array<int, 3>(
                {std::stoi(parsedVersionNumberComponents[0]),
                 std::stoi(parsedVersionNumberComponents[1]),
                 std::stoi(parsedVersionNumberComponents[2])});
            if (webYear > parsedYear) {
                webVersionIsGreater = true;
            } else if (webYear == parsedYear && webMonth > parsedMonth) {
                webVersionIsGreater = true;
            } else if (webYear == parsedYear && webMonth == parsedMonth
                       && webDay > parsedDay) {
                webVersionIsGreater = true;
            } else {
                webVersionIsGreater = false;
            }
        }

        if (!webVersionIsGreater) {
            continue;
        }

        if (!sourceObject.contains("updateLink")) {
            std::cerr
                << "Fetched object does not contain updateLink field, skipping"
                << std::endl;
            continue;
        }
        std::string updateLink
            = sourceObject.value("updateLink").toString().toStdString();

        if (!sourceObject.contains("checksum")) {
            std::cerr
                << "Fetched object does not contain checksum field, skipping"
                << std::endl;
            continue;
        }
        std::string checksum
            = sourceObject.value("checksum").toString().toStdString();

        QJsonValue desc = sourceObject.value("description");

        availability[sourceName] = {
            .sourceName = sourceName,
            .versionNumber = webVersionNumber,
            .url = updateLink,
            .checksum = checksum,
            .description = (desc == QJsonValue::Undefined)
                               ? std::nullopt
                               : std::optional{desc.toString().toStdString()},
        };
    }

    return true;
}

void SourceReleaseChecker::parseReply(QNetworkReply *reply)
{
    std::string content = reply->readAll().toStdString();
    if (content.empty()) {
        return;
    }

    bool updateAvailable;
    std::unordered_map<std::string, IUpdateChecker::SourceManifestMetadata>
        updates;
    if (parseJSON(content, updates)) {
        for (const auto &[s, u] : updates) {
            _updates.emplace_back(u);
        }
    }
}

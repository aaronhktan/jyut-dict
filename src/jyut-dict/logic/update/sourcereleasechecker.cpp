#include "sourcereleasechecker.h"

#include "logic/utils/utils.h"

#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>

#include <iostream>

namespace {
constexpr auto DEFAULT_SOURCE_UPDATE_URL
    = "https://jyutdictionary.com/static/updates/v1/sources_manifest.json";

constexpr auto VERSION_NUMBER_COMPONENTS_SIZE = 3;
} // namespace

SourceReleaseChecker::SourceReleaseChecker(
    std::shared_ptr<SQLDatabaseManager> manager, QObject *parent)
    : _databaseManager{manager}
    , QObject{parent}
{
    _utils = std::make_unique<SQLDatabaseUtils>(_databaseManager);
    _networkManager = new QNetworkAccessManager{this};

    QSslSocket::supportsSsl();
}

void SourceReleaseChecker::checkForNewUpdate()
{
    std::vector<DictionaryMetadata> sources;
    _utils->readSources(sources);

    for (const auto &s : sources) {
        _sourceMetadata[s.getName()] = s;
        _sourceUpdateURLs.emplace(s.getUpdateURL().empty()
                                      ? DEFAULT_SOURCE_UPDATE_URL
                                      : s.getUpdateURL());
    }

    for (const auto &u : _sourceUpdateURLs) {
        QNetworkRequest request{QUrl{u.c_str()}};
        QNetworkReply *reply = _networkManager->get(request);
        _replies.emplace(reply);
        connect(reply,
                &QNetworkReply::finished,
                this,
                [this, u](QNetworkReply *reply) {
                    parseReply(reply);
                    _sourceUpdateURLs.erase(u);
                    if (_sourceUpdateURLs.empty()) {
                        emit foundUpdate(_updates);
                    }
                });

        // Time out after 15 seconds
        QTimer::singleShot(15000, this, [this, u, reply]() {
            _sourceUpdateURLs.erase(u);
            disconnect(reply, nullptr, nullptr, nullptr);
            reply->deleteLater();
            _replies.erase(reply);

            if (_sourceUpdateURLs.empty()) {
                emit foundUpdate(_updates);
            }
        });
    }
}

bool SourceReleaseChecker::parseJSON(
    const std::string &data,
    std::unordered_map<std::string, IUpdateChecker::SourceUpdateAvailability>
        &availability)
{
    QJsonDocument doc = QJsonDocument::fromJson(
        QString::fromStdString(data.c_str()).toUtf8());
    QJsonValue source;
    for (const auto &source : doc.array()) {
        QJsonObject sourceObject = source.toObject();

        // Check if the user_version is supported
        int userVersion{sourceObject.value("userVersion").toInt()};
        if (userVersion > CURRENT_DATABASE_VERSION) {
            continue;
        }

        // Check if sourceName corresponds to an installed dictionary
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
            return false;
        }
        auto [year, month, day] = std::array<int, 3>(
            {std::stoi(sourceVersionNumberComponents[0]),
             std::stoi(sourceVersionNumberComponents[1]),
             std::stoi(sourceVersionNumberComponents[2])});

        // Check if version on the web is larger than local version
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
            Utils::split(availability.at(sourceName)
                             .versionNumber.value_or("0-0-0"),
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

        std::string updateLink
            = sourceObject.value("updateLink").toString().toStdString();
        std::string description
            = sourceObject.value("description").toString().toStdString();

        availability[sourceName] = {
            .updateAvailable = true,
            .sourceName = sourceName,
            .versionNumber = webVersionNumber,
            .url = updateLink,
            .description = description,
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
    std::unordered_map<std::string, IUpdateChecker::SourceUpdateAvailability>
        updates;
    if (parseJSON(content, updates)) {
        for (const auto &[s, u] : updates) {
            _updates.emplace_back(u);
        }
    }

    reply->deleteLater();
    _replies.erase(reply);
}

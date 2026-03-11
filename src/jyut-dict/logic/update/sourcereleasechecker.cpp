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

// TODO: Actually implement this section
bool parseJSON(const std::string &data,
               bool &updateAvailable,
               std::string &versionNumber,
               std::string &url,
               std::string &description)
{
    std::vector<std::string> localVersionNumberComponents;
    Utils::split(std::string{Utils::CURRENT_VERSION},
                 "-",
                 localVersionNumberComponents);
    if (localVersionNumberComponents.size() != 3) {
        std::cerr << "Expected current versionNumber to have "
                  << VERSION_NUMBER_COMPONENTS_SIZE << ", got "
                  << localVersionNumberComponents.size();
        return false;
    }
    auto [localMajor, localMinor, localPatch] = std::array<int, 3>(
        {std::stoi(localVersionNumberComponents[0]),
         std::stoi(localVersionNumberComponents[1]),
         std::stoi(localVersionNumberComponents[2])});

    QJsonDocument doc = QJsonDocument::fromJson(
        QString::fromStdString(data.c_str()).toUtf8());
    QJsonValue version;
    for (const auto &version : doc.array()) {
        QJsonObject versionObject = version.toObject();

        // Check if the version on the web is for the correct channel
        QString channel{versionObject.value("channel").toString()};

        QString applicationChannel = QLibraryInfo::isDebugBuild() ? "debug"
                                                                  : "release";
#if defined(Q_OS_MAC) && defined(DEBUG)
        // isDebugBuild() always returns false on macOS. For more information, see
        // https://stackoverflow.com/questions/11714118/detect-if-qt-is-running-a-debug-build-at-runtime
        applicationChannel = "debug";
#endif
        if (applicationChannel != channel) {
            continue;
        }

        // Check if version on the web is larger than local version
        bool webVersionIsGreater = false;
        std::string webVersionNumber{
            versionObject.value("versionNumber").toString().toStdString()};
        std::vector<std::string> webVersionNumberComponents;
        Utils::split(webVersionNumber, ".", webVersionNumberComponents);
        if (webVersionNumberComponents.size() != 3) {
            std::cerr << "Expected web versionNumber to have "
                      << VERSION_NUMBER_COMPONENTS_SIZE << ", got "
                      << webVersionNumberComponents.size();
            continue;
        }
        auto [webMajor, webMinor, webPatch] = std::array<int, 3>(
            {std::stoi(webVersionNumberComponents[0]),
             std::stoi(webVersionNumberComponents[1]),
             std::stoi(webVersionNumberComponents[2])});
        if (webMajor > localMajor) {
            // If web major version if larger than local major version,
            // an update was found
            webVersionIsGreater = true;
        } else if (webMajor == localMajor && webMinor > localMinor) {
            // Major versions are equal, but web minor version if higher
            // than local minor version, so an update was found
            webVersionIsGreater = true;
        } else if (webMajor == localMajor && webMinor == localMinor
                   && webPatch > localPatch) {
            // Major and minor versions are the same, but web patch version
            // is higher than local patch version, so an update was found
            webVersionIsGreater = true;
        } else {
            webVersionIsGreater = false;
        }

        if (!webVersionIsGreater) {
            continue;
        }

        QJsonValue link;
        for (const auto &link : versionObject.value("links").toArray()) {
            QJsonObject linkObject = link.toObject();

            // Target kernel must match
            QString webKernel = linkObject.value("kernel").toString();
            if (webKernel != QSysInfo::kernelType()) {
                continue;
            }

            // OS must be compatible
            bool doubleConversionStatus;
            auto webOSVersion = linkObject.value("minOSVersionNumber")
                                    .toString()
                                    .toDouble(&doubleConversionStatus);
            if (!doubleConversionStatus) {
                continue;
            }
            auto currentOSVersion = QSysInfo::productVersion().toDouble(
                &doubleConversionStatus);
            if (!doubleConversionStatus) {
                continue;
            }
            if (currentOSVersion < webOSVersion) {
                continue;
            }

            // At least one architecture must match the local architecture
            bool matchesArch = false;
            QJsonValue webArch;
            for (const auto &webArch : linkObject.value("arch").toArray()) {
                if (webArch.toString() == QSysInfo::buildCpuArchitecture()) {
                    matchesArch = true;
                    break;
                }
            }

            if (!matchesArch) {
                continue;
            }

            // Variant must match
            QString webVariant = linkObject.value("variant").toString();
            if (webVariant != Utils::VARIANT) {
                continue;
            }

#ifdef Q_OS_LINUX
            // On Linux, the install versions (aka .deb) must be built for
            // the specific version of Ubuntu
            if (webVariant == "install" && webOSVersion != currentOSVersion) {
                continue;
            }
#endif

            // If all these conditions are true, then this is a valid update!
            updateAvailable = true;
            versionNumber = webVersionNumber;
            url = linkObject.value("updateLink").toString().toStdString();
            description
                = versionObject.value("description").toString().toStdString();
            return true;
        }
    }

    updateAvailable = false;
    return true;
}

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

    // TODO: fix this part; how to do multiple requests at same time + coalesce results?
    for (const auto &u : _sourceUpdateURLs) {
        QNetworkRequest _request{QUrl{u.c_str()}};
        disconnect(_networkManager, nullptr, nullptr, nullptr);
        connect(_networkManager,
                &QNetworkAccessManager::finished,
                this,
                &SourceReleaseChecker::parseReply);
        _reply = _networkManager->get(_request);
        QTimer::singleShot(15000, this, [&]() { emit foundUpdate({}); });
    }
}

void SourceReleaseChecker::parseReply(QNetworkReply *reply)
{
    std::string content = reply->readAll().toStdString();
    if (content.empty()) {
        return;
    }

    bool updateAvailable;
    std::string url, versionNumber, description;
    if (parseJSON(content, updateAvailable, versionNumber, url, description)) {
        emit foundUpdate(IUpdateChecker::AppUpdateAvailability{
            .updateAvailable = updateAvailable,
            .versionNumber = versionNumber,
            .url = url,
            .description = description,
        });
    }
}

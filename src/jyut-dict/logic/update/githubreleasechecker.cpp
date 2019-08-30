#include "githubreleasechecker.h"

#include "logic/settings/settingsutils.h"
#include "logic/utils/utils.h"

#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>

#include <regex>

GithubReleaseChecker::GithubReleaseChecker(QObject *parent)
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
    QTimer::singleShot(100, this, &GithubReleaseChecker::preConnectToHost);
}

GithubReleaseChecker::~GithubReleaseChecker()
{

}

void GithubReleaseChecker::checkForNewUpdate()
{
    QNetworkRequest _request{QUrl{GITHUB_UPDATE_URL}};
    _reply = _manager->get(_request);
    disconnect(_manager, nullptr, nullptr, nullptr);
    connect(_manager, &QNetworkAccessManager::finished, this, &GithubReleaseChecker::parseReply);
    QTimer::singleShot(15000, this, [&]() {
        emit foundUpdate(false, "", "", "");
    });
}

void GithubReleaseChecker::preConnectToHost()
{
    _manager->connectToHostEncrypted(QUrl{GITHUB_UPDATE_URL}.host());
}

void GithubReleaseChecker::parseReply(QNetworkReply *reply)
{
    std::string content = reply->readAll().toStdString();
    // ConnectToHostEncrypted gets an empty string reply, can safely ignore
    if (content.empty()) {
        return;
    }

    bool updateAvailable;
    std::string url, versionNumber, description;
    if (parseJSON(content, updateAvailable, versionNumber, url, description)) {
        emit foundUpdate(updateAvailable, versionNumber, url, description);
    }
}

bool GithubReleaseChecker::parseJSON(const std::string &data,
                                     bool &updateAvailable,
                                     std::string &versionNumber,
                                     std::string &url, std::string &description)
{
    QJsonDocument doc = QJsonDocument::fromJson(QString{data.c_str()}.toUtf8());
    for (QJsonValue entry : doc.array()) {
        std::string release_name = entry.toObject().value("name")
                .toString().toStdString();

        // Regex has three capturing groups:
        // The first is for platform, which is one string of any characters
        // The first and second capturing groups are separated by whitespace
        // The second is the version number
        // The second and third capturing groups are separated by a "\"
        // The third capturing group is the channel
        std::regex release_regex{"(.*)\\s.?(.*)\\-(.*)"};
        std::smatch release_match;

        if (!std::regex_match(release_name, release_match, release_regex)) {
            continue;
        }

        // Expect three matches, but for std::smatch, first element is whole string.
        if (release_match.size() != 4) {
            return false;
        }

        std::string platform = release_match[1].str();
        std::string version = release_match[2].str();
        std::string channel = release_match[3].str();

        if (platform.find(Utils::PLATFORM_NAME) == std::string::npos) {
            continue;
        }

        std::vector<std::string> new_version;
        Utils::split(version, '.', new_version);

        std::vector<std::string> current_version;
        Utils::split(Utils::CURRENT_VERSION, '.', current_version);

        for (size_t i = 0; i < new_version.size(); i++) {
            // Check next section if semver section is equal

            // Current version is newer than downloaded version if any of the
            // semver parts are higher, i.e. it's a newer version
            if (std::stoi(current_version.at(i)) > std::stoi(new_version.at(i))) {
                break;
            }

            // Current version is older than downloaded version if any of the
            // semver parts are lower
            if (std::stoi(current_version.at(i)) < std::stoi(new_version.at(i))) {
                // Check each browser download url for a compatible file extension
                // Set the url to the first one that matches
                for (QJsonValue file : entry.toObject().value("assets").toArray()) {
                    std::string download_url = file.toObject().value("browser_download_url")
                            .toString().toStdString();
                    if (std::any_of(Utils::ASSET_FORMATS.begin(),
                                    Utils::ASSET_FORMATS.end(),
                                    [&](std::string substring) {
                                        return download_url.find(substring) != std::string::npos;
                                    })
                        && download_url.find(Utils::ARCHITECTURE) != std::string::npos
                        && download_url.find(Utils::PORTABILITY) != std::string::npos
                        && download_url.find(Utils::PLATFORM_NAME) != std::string::npos
#if defined(Q_OS_WIN) && !defined(PORTABLE)
                        && download_url.find(Settings::getCurrentLocaleLanguageAndScriptIfChinese()) != std::string::npos
#endif
                        ) {
                        url = download_url;
                        versionNumber = version;
                        description = entry.toObject().value("body").toString().toStdString();
                        updateAvailable = true;
                        return true;
                    }
                }

                break;
            }
        }
    }

    updateAvailable = false;
    return true;
}

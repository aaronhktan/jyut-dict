#include "logic/search/searchparameters.h"
#include "logic/strings/strings.h"

#include <QColor>
#include <QCoreApplication>
#include <QFileInfo>
#include <QLocale>
#include <QStandardPaths>

#include <unordered_map>

namespace Utils {

static std::unordered_map<std::string, QColor> colourMap = {
    {"cmn", QColor{14, 139, 83}},
    {"eng", QColor{100, 76, 143}},
    {"fra", QColor{0, 48, 143}},
    {"deu", QColor{186, 200, 95}},
    {"yue", QColor{173, 31, 90}},
};

static std::unordered_map<std::string, QString> languageMap = {
    {"cmn",
     QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::CMN_STRING)},
    {"eng",
     QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::ENG_STRING)},
    {"fra",
     QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::FRA_STRING)},
    {"deu",
     QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::DEU_STRING)},
    {"yue",
     QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::YUE_STRING)},
    {"zho",
     QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::ZHO_STRING)},
};

static QMap<QString, std::string> reverseLanguageMap = {
    {QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::CMN_STRING),
     "cmn"},
    {QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::ENG_STRING),
     "eng"},
    {QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::FRA_STRING),
     "fra"},
    {QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::DEU_STRING),
     "deu"},
    {QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::YUE_STRING),
     "yue"},
    {QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::ZHO_STRING),
     "zho"},
};

static QMap<SearchParameters, std::string> searchParameterMap = {
    {SearchParameters::SIMPLIFIED,
     QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::SC_STRING)
         .toStdString()},
    {SearchParameters::TRADITIONAL,
     QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::TC_STRING)
         .toStdString()},
    {SearchParameters::CHINESE,
     QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::CN_STRING)
         .toStdString()},
    {SearchParameters::JYUTPING,
     QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::JP_STRING)
         .toStdString()},
    {SearchParameters::PINYIN,
     QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::PY_STRING)
         .toStdString()},
    {SearchParameters::ENGLISH,
     QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::EN_STRING)
         .toStdString()},
};

QColor getContrastingColour(const QColor backgroundColour)
{
    auto brightness = backgroundColour.redF() * 0.299
                      + backgroundColour.greenF() * 0.587
                      + backgroundColour.blueF() * 0.114;
    return (brightness > 0.65) ? QColor{0, 0, 0} : QColor{0xFF, 0xFF, 0xFF};
}

QColor getLanguageColour(std::string language)
{
    QColor colour;
    try {
        colour = colourMap.at(language);
    } catch (std::out_of_range &e) {
        colour = QColor{"slategrey"};
    }
    return colour;
}

QString getLanguageFromISO639(std::string language)
{
    QString result;
    try {
        result = languageMap.at(language);
    } catch (std::out_of_range &e) {
        QLocale locale{QString{language.c_str()}};
        result = locale.languageToString(locale.language());
    }
    return result;
}

std::string getISO639FromLanguage(QString language)
{
    std::string result;
    if (reverseLanguageMap.contains(language)) {
        result = reverseLanguageMap[language];
    } else {
        QString str = QLocale{language}.name();
        result = str.mid(0, str.indexOf("_")).toStdString();
    }
    return result;
}

std::string getStringFromSearchParameter(SearchParameters parameter)
{
    std::string result;
    if (searchParameterMap.contains(parameter)) {
        result = searchParameterMap[parameter];
    } else {
        result = "";
    }
    return result;
}

void refreshLanguageMap()
{
    languageMap = {
        {"cmn",
         QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                     Strings::CMN_STRING)},
        {"eng",
         QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                     Strings::ENG_STRING)},
        {"fra",
         QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                     Strings::FRA_STRING)},
        {"deu",
         QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                     Strings::DEU_STRING)},
        {"yue",
         QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                     Strings::YUE_STRING)},
        {"zho",
         QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                     Strings::ZHO_STRING)},
    };

    reverseLanguageMap = {
        {QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                     Strings::CMN_STRING),
         "cmn"},
        {QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                     Strings::ENG_STRING),
         "eng"},
        {QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                     Strings::FRA_STRING),
         "fra"},
        {QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                     Strings::DEU_STRING),
         "deu"},
        {QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                     Strings::YUE_STRING),
         "yue"},
        {QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                     Strings::ZHO_STRING),
         "zho"},
    };

    searchParameterMap = {
        {SearchParameters::SIMPLIFIED,
         QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                     Strings::SC_STRING)
             .toStdString()},
        {SearchParameters::TRADITIONAL,
         QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                     Strings::TC_STRING)
             .toStdString()},
        {SearchParameters::CHINESE,
         QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                     Strings::CN_STRING)
             .toStdString()},
        {SearchParameters::JYUTPING,
         QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                     Strings::JP_STRING)
             .toStdString()},
        {SearchParameters::PINYIN,
         QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                     Strings::PY_STRING)
             .toStdString()},
        {SearchParameters::ENGLISH,
         QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                     Strings::EN_STRING)
             .toStdString()},
    };
}

QString getLocalLicenseFolderPath()
{
#ifdef Q_OS_DARWIN
    QFileInfo localFile{
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
        + "/licenses/"};
#elif defined(Q_OS_WIN)
    QFileInfo localFile{
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
        + "/licenses/"};
#else
    QFileInfo localFile{
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
        + "/licenses/"};
#endif
    return localFile.absoluteFilePath();
}

QString getBundleLicenseFolderPath()
{
#ifdef Q_OS_DARWIN
    QFileInfo bundleFile{QCoreApplication::applicationDirPath()
                         + "/../Resources/licenses/"};
#elif defined(Q_OS_WIN)
    QFileInfo bundleFile{QCoreApplication::applicationDirPath() + "./"};
#else
#ifdef APPIMAGE
    QFileInfo bundleFile{QCoreApplication::applicationDirPath()
                         + "/../share/jyut-dict/licenses/"};
#elif defined(DEBUG)
    QFileInfo bundleFile{"./"};
#else
    QFileInfo bundleFile{"/usr/share/jyut-dict/licenses/"};
#endif
#endif
    return bundleFile.absoluteFilePath();
}

QString getLicenseFolderPath()
{
#ifdef PORTABLE
    return getBundleLicenseFolderPath();
#else
    return getLocalLicenseFolderPath();
#endif
}
} // namespace Utils

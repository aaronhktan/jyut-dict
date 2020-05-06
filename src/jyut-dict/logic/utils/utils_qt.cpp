#include "utils.h"

#include "logic/strings/strings.h"

#include <QCoreApplication>
#include <QColor>
#include <QLocale>

#include <unordered_map>

#include <QDebug>

namespace Utils {

static std::unordered_map<std::string, QColor> colourMap = {
    {"cmn", QColor{6, 56, 33}},
    {"eng", QColor{100, 76, 143}},
    {"fra", QColor{0, 48, 143}},
    {"deu", QColor{186, 200, 95}},
    {"yue", QColor{73, 13, 38}},
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
}

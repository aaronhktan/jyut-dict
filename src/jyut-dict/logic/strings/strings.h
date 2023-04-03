#ifndef STRINGS_H
#define STRINGS_H

#include <QTranslator>

// Strings should contain all the strings that need to be translated
// and are used in several different locations.

// To use these, #include <QCoreApplication>, then
// QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::<define here>)

namespace Strings {

constexpr auto STRINGS_CONTEXT = "strings";

constexpr auto JYUTPING_SHORT = QT_TRANSLATE_NOOP("strings", "JP");
constexpr auto YALE_SHORT = QT_TRANSLATE_NOOP("strings", "YL");
constexpr auto CANTONESE_IPA_SHORT = QT_TRANSLATE_NOOP("strings", "YI");
constexpr auto PINYIN_SHORT = QT_TRANSLATE_NOOP("strings", "PY");
constexpr auto ZHUYIN_SHORT = QT_TRANSLATE_NOOP("strings", "ZY");
constexpr auto MANDARIN_IPA_SHORT = QT_TRANSLATE_NOOP("strings", "ZI");
constexpr auto DEFINITIONS_ALL_CAPS = QT_TRANSLATE_NOOP("strings",
                                                        "DEFINITIONS");
constexpr auto SENTENCES_ALL_CAPS = QT_TRANSLATE_NOOP("strings", "SENTENCES");
constexpr auto RELATED_ALL_CAPS = QT_TRANSLATE_NOOP("strings", "RELATED");

constexpr auto PRODUCT_NAME = QT_TRANSLATE_NOOP("strings", "Jyut Dictionary");
constexpr auto PRODUCT_DESCRIPTION
    = QT_TRANSLATE_NOOP("strings",
                        "A free, open-source offline Cantonese Dictionary.");
constexpr auto CREDITS_TEXT = QT_TRANSLATE_NOOP(
    "strings",
    "Icon based on <a "
    "href=https://commons.wikimedia.org/wiki/"
    "Commons:Ancient_Chinese_characters_project "
    "style=\"color: %1; text-decoration: none\">Wikimedia Ancient "
    "Chinese characters project</a> files."
    "<br>UI icons from <a href=https://feathericons.com "
    "style=\"color: %1; text-decoration: none\">Feather Icons</a> and "
    "<a href=https://github.com/microsoft/fluentui-system-icons "
    "style=\"color: %1; text-decoration: none\">Fluent UI System Icons</a>."
    "<br>Contains FlowLayout code from <a href=\"file://%2\" style=\"color: "
    "%1; text-decoration: none\">Qt</a>."
    "<br>Made with <a href=\"#\" style=\"color: %1; text-decoration: none\">"
    "Qt</a>.");
constexpr auto CONTACT_TITLE
    = QT_TRANSLATE_NOOP("strings",
                        "%1 is made with ❤️ by Aaron Tan.");
constexpr auto CONTACT_BODY
    = QT_TRANSLATE_NOOP("strings",
                        "Donate 💵, report a bug 🐛, or just say hi 👋!");
constexpr auto CONTACT_BODY_NO_EMOJI
    = QT_TRANSLATE_NOOP("strings", "Donate, report a bug, or just say hi!");
constexpr auto OTHER_SOURCES = QT_TRANSLATE_NOOP(
    "strings",
    "<a href=https://www.pleco.com style=\"color: %1; "
    "text-decoration: none\">Pleco Dictionary</a>, "
    "<a href=http://www.cantonese.sheik.co.uk/dictionary/ "
    "style=\"color: %1; text-decoration: none\">CantoDict</a>, "
    "<a href=https://words.hk style=\"color: %1; text-decoration: "
    "none\">words.hk</a>");
constexpr auto YUE_ERROR_STRING = QT_TRANSLATE_NOOP(
    "strings",
    "A voice for \"Chinese - Hong Kong\" could not be found.");
constexpr auto YUE_DESCRIPTION_STRING
    = QT_TRANSLATE_NOOP("strings",
                        "Unable to use text to speech.\n\n"
                        "Debug: Locale: %1, Language: Chinese - Hong Kong");

constexpr auto ZH_TW_ERROR_STRING = QT_TRANSLATE_NOOP(
    "strings",
    "A voice for \"Chinese - Taiwan\" could not be found.");
constexpr auto ZH_TW_DESCRIPTION_STRING
    = QT_TRANSLATE_NOOP("strings",
                        "Unable to use text to speech.\n\n"
                        "Debug: Locale: %1, Language: Chinese - Taiwan");

constexpr auto ZH_CN_ERROR_STRING = QT_TRANSLATE_NOOP(
    "strings",
    "A voice for \"Chinese - Mainland China\" could not be found.");
constexpr auto ZH_CN_DESCRIPTION_STRING = QT_TRANSLATE_NOOP(
    "strings",
    "Unable to use text to speech.\n\n"
    "Debug: Locale: %1, Language: Chinese - Mainland");

constexpr auto CANTONESE_REFERENCE_URL = QT_TRANSLATE_NOOP(
    "strings",
    "<a href=https://en.wikipedia.org/wiki/Help:IPA/Cantonese style=\"color: "
    "%1; text-decoration: none\">Learn more with the Cantonese "
    "Pronunciation Reference →</a>");
constexpr auto LEARN_JYUTPING_URL = QT_TRANSLATE_NOOP(
    "strings",
    "<a href=https://jyutping.org/en/docs/english/ style=\"color: "
    "%1; text-decoration: none\">Learn Jyutping →</a>");
constexpr auto LEARN_YALE_URL = QT_TRANSLATE_NOOP(
    "strings",
    "<a "
    "href=https://opencantonese.org/cantonese-pronunciation-yale/introduction/"
    "overview-cantonese-sounds style=\"color: %1; text-decoration: "
    "none\">Learn Yale →</a>");
constexpr auto LEARN_CANTONESE_IPA_URL = QT_TRANSLATE_NOOP(
    "strings",
    "<a href=https://cantonese.ca/pronunciation.html style=\"color: "
    "%1; text-decoration: none\">Learn Cantonese IPA →</a>");
constexpr auto MANDARIN_REFERENCE_URL = QT_TRANSLATE_NOOP(
    "strings",
    "<a href=https://en.wikipedia.org/wiki/Help:IPA/Mandarin style=\"color: "
    "%1; text-decoration: none\">Learn more with the Mandarin "
    "Pronunciation Reference →</a>");
constexpr auto LEARN_PINYIN_URL = QT_TRANSLATE_NOOP(
    "strings",
    "<a href=https://www.dong-chinese.com/learn/sounds/pinyin style=\"color: "
    "%1; text-decoration: none\">Learn Pinyin →</a>");
constexpr auto LEARN_BOPOMOFO_URL = QT_TRANSLATE_NOOP(
    "strings",
    "<a href=https://www.dong-chinese.com/learn/sounds/zhuyin style=\"color: "
    "%1; text-decoration: none\">Learn Zhuyin →</a>");
constexpr auto LEARN_MANDARIN_IPA_URL = QT_TRANSLATE_NOOP(
    "strings",
    "<a href=https://www.internationalphoneticassociation.org/IPAcharts/"
    "IPA_chart_trans/pdfs/JIPA_2011_[2007]_zho.pdf style=\"color: %1; "
    "text-decoration: none\">Learn Mandarin IPA →</a>");

constexpr auto ENG_STRING = QT_TRANSLATE_NOOP("strings", "English");
constexpr auto FRA_STRING = QT_TRANSLATE_NOOP("strings", "French");
constexpr auto DEU_STRING = QT_TRANSLATE_NOOP("strings", "German");
constexpr auto YUE_STRING = QT_TRANSLATE_NOOP("strings", "Cantonese");
constexpr auto CMN_STRING = QT_TRANSLATE_NOOP("strings", "Mandarin");
constexpr auto ZHO_STRING = QT_TRANSLATE_NOOP("strings", "Chinese");

constexpr auto SC_STRING = QT_TRANSLATE_NOOP("strings", "SC");
constexpr auto TC_STRING = QT_TRANSLATE_NOOP("strings", "TC");
constexpr auto CN_STRING = QT_TRANSLATE_NOOP("strings", "CN");
constexpr auto JP_STRING = QT_TRANSLATE_NOOP("strings", "JP");
constexpr auto PY_STRING = QT_TRANSLATE_NOOP("strings", "PY");
constexpr auto EN_STRING = QT_TRANSLATE_NOOP("strings", "EN");
} // namespace Strings

#endif // STRINGS_H

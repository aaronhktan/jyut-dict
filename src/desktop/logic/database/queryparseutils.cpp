#include "queryparseutils.h"

#include "logic/entry/entry.h"
#include "logic/example/example.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlQuery>
#include <QSqlRecord>

namespace QueryParseUtils {

std::vector<Entry> parseEntries(QSqlQuery &query, bool parseDefinitions)
{
    std::vector<Entry> entries;

    const int simplifiedIndex = query.record().indexOf("simplified");
    const int traditionalIndex = query.record().indexOf("traditional");
    const int jyutpingIndex = query.record().indexOf("jyutping");
    const int pinyinIndex = query.record().indexOf("pinyin");
    const int definitionIndex = parseDefinitions
                                    ? query.record().indexOf("definitions")
                                    : 0;

    while (query.next()) {
        // Get fields from table
        const std::string simplified
            = query.value(simplifiedIndex).toString().toStdString();
        const std::string traditional
            = query.value(traditionalIndex).toString().toStdString();
        const std::string jyutping
            = query.value(jyutpingIndex).toString().toStdString();
        const std::string pinyin
            = query.value(pinyinIndex).toString().toStdString();
        const std::string definition
            = query.value(definitionIndex).toString().toStdString();
        if (definition.empty()) {
            continue;
        }

        // Put definitions in the correct DefinitionsSet
        std::vector<DefinitionsSet> definitionsSets;

        if (parseDefinitions) {
            // Parse JSON returned by query
            QJsonDocument doc = QJsonDocument::fromJson(
                QString::fromStdString(definition).toUtf8());
            // Each object in the array represents a group of definitions
            // that are all from the same source
            // We can ignore the C++ range-loop error because Qt no longer supports
            // using foreach with QJsonValue
            for (const QJsonValue &definitionGroup : doc.array()) {
                const std::string sourceName
                    = definitionGroup["source"].toString().toStdString();
                std::vector<Definition::Definition> definitions;

                for (const QJsonValue &definition :
                     definitionGroup["definitions"].toArray()) {
                    std::vector<Example> examples;
                    for (const QJsonValue &example :
                         definition["examples"].toArray()) {
                        std::vector<TranslationSet> example_translations;

                        // Parse each example
                        if (!example.isNull()) {
                            std::vector<Translation::Translation> targetTranslations;
                            if (!example["translations"].isNull()) {
                                // Parse each of the example translations
                                for (const QJsonValue &translation :
                                     example["translations"].toArray()) {
                                    targetTranslations.emplace_back(
                                        translation["translation"]
                                            .toString()
                                            .toStdString(),
                                        translation["language"]
                                            .toString()
                                            .toStdString(),
                                        translation["direct"].toInt() == 1);
                                }
                                example_translations.emplace_back(sourceName, targetTranslations);
                            }

                            examples.emplace_back(example["language"].toString().toStdString(),
                                                   example["simplified"].toString().toStdString(),
                                                   example["traditional"].toString().toStdString(),
                                                   example["jyutping"].toString().toStdString(),
                                                   example["pinyin"].toString().toStdString(),
                                                   example_translations);
                        }
                    }

                    definitions.emplace_back(definition["definition"]
                                                 .toString()
                                                 .replace("ﾠ", " ")
                                                 .toStdString(),
                                             definition["label"]
                                                 .toString()
                                                 .toStdString(),
                                             examples);
                }

                definitionsSets.emplace_back(sourceName,
                                             definitions);
            }
        }

        entries.emplace_back(simplified,
                             traditional,
                             jyutping,
                             pinyin,
                             definitionsSets);
    }

    return entries;
}

std::vector<Example> parseExamples(QSqlQuery &query)
{
    std::vector<Example> examples;

    const int simplifiedIndex = query.record().indexOf("simplified");
    const int traditionalIndex = query.record().indexOf("traditional");
    const int jyutpingIndex = query.record().indexOf("jyutping");
    const int pinyinIndex = query.record().indexOf("pinyin");
    const int sourceLanguageIndex = query.record().indexOf("language");
    const int translationsIndex = query.record().indexOf("translations");
    const int definitionSourceNameIndex = query.record().indexOf("sourcename");

    while (query.next()) {
        // Get fields from table
        const std::string simplified
            = query.value(simplifiedIndex).toString().toStdString();
        const std::string traditional
            = query.value(traditionalIndex).toString().toStdString();
        const std::string jyutping
            = query.value(jyutpingIndex).toString().toStdString();
        const std::string pinyin
            = query.value(pinyinIndex).toString().toStdString();
        const std::string sourceLanguage
            = query.value(sourceLanguageIndex).toString().toStdString();
        const std::string combinedTargetTranslationsData
            = query.value(translationsIndex).toString().toStdString();
        const std::string definitionSourceName
            = query.value(definitionSourceNameIndex).toString().toStdString();

        // Each example will have a vector of TranslationSets that represents
        // a group of translations that are from the same source
        std::vector<TranslationSet> translationSets;

        if (!combinedTargetTranslationsData.empty()) {
            // Parse JSON returned by query
            QJsonDocument doc = QJsonDocument::fromJson(
                QString::fromStdString(combinedTargetTranslationsData).toUtf8());
            for (const QJsonValue &translation_set : doc.array()) {
                const std::string translationSourceName
                    = translation_set["source"].toString().toStdString();

                // If the example has the source it comes from, use it
                // Otherwise, assume it comes from the same source as the
                // definition it is linked to (if any)
                translationSets.emplace_back(translationSourceName.empty()
                                                 ? definitionSourceName
                                                 : translationSourceName);

                for (const QJsonValue &translation :
                     translation_set["translations"].toArray()) {
                    // Parse each translation in this group
                    std::vector<Translation::Translation> targetTranslations;
                    translationSets.back().pushTranslation(
                        {translation["translation"].toString().toStdString(),
                         translation["language"].toString().toStdString(),
                         translation["direct"].toInt() == 1});
                }
            }
        }

        examples.emplace_back(sourceLanguage,
                              simplified,
                              traditional,
                              jyutping,
                              pinyin,
                              translationSets);
    }

    return examples;
}

bool parseExistence(QSqlQuery &query)
{
    bool existence = false;

    const int existenceIndex = query.record().indexOf("existence");

    while (query.next()) {
        existence = query.value(existenceIndex).toInt() == 1;
    }
    return existence;
}

std::vector<searchTermHistoryItem> parseHistoryItems(QSqlQuery &query)
{
    std::vector<searchTermHistoryItem> results;

    const int textIndex = query.record().indexOf("text");
    const int optionsIndex = query.record().indexOf("options");

    while (query.next()) {
        std::string text = query.value(textIndex).toString().toStdString();
        int options = query.value(optionsIndex).toInt();

        results.push_back(searchTermHistoryItem{text, options});
    }

    return results;
}

}

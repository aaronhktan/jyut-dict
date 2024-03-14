#include "queryparseutils.h"

#include "logic/utils/utils.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlRecord>

namespace QueryParseUtils {

std::vector<Entry> parseEntries(QSqlQuery &query, bool parseDefinitions)
{
    std::vector<Entry> entries;

    int simplifiedIndex = query.record().indexOf("simplified");
    int traditionalIndex = query.record().indexOf("traditional");
    int jyutpingIndex = query.record().indexOf("jyutping");
    int pinyinIndex = query.record().indexOf("pinyin");
    int definitionIndex = parseDefinitions ?
                query.record().indexOf("definitions") : 0;

    while (query.next()) {
        // Get fields from table
        std::string simplified
            = query.value(simplifiedIndex).toString().toStdString();
        std::string traditional
            = query.value(traditionalIndex).toString().toStdString();
        std::string jyutping
            = query.value(jyutpingIndex).toString().toStdString();
        std::string pinyin = query.value(pinyinIndex).toString().toStdString();
        std::string definition
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
            foreach (const QJsonValue &definitionGroup, doc.array()) {

                std::string sourceName = definitionGroup["source"].toString().toStdString();
                std::vector<Definition::Definition> definitions;

                foreach (const QJsonValue &definition, definitionGroup["definitions"].toArray()) {

                    std::vector<SourceSentence> sentences;
                    foreach (const QJsonValue &sentence, definition["sentences"].toArray()) {
                        std::vector<SentenceSet> sentence_translations;

                        // Parse each sentence
                        if (!sentence.isNull()) {
                            std::vector<Sentence::TargetSentence> targetSentences;
                            if (!sentence["translations"].isNull()) {
                                // Parse each of the sentence translations
                                foreach (const QJsonValue &translation, sentence["translations"].toArray()) {
                                    targetSentences.emplace_back(
                                        translation["sentence"]
                                            .toString()
                                            .toStdString(),
                                        translation["language"]
                                            .toString()
                                            .toStdString(),
                                        translation["direct"].toInt() == 1);
                                }
                                sentence_translations.emplace_back(sourceName, targetSentences);
                            }

                            sentences.emplace_back(sentence["language"].toString().toStdString(),
                                                   sentence["simplified"].toString().toStdString(),
                                                   sentence["traditional"].toString().toStdString(),
                                                   sentence["jyutping"].toString().toStdString(),
                                                   sentence["pinyin"].toString().toStdString(),
                                                   sentence_translations);
                        }
                    }

                    definitions.emplace_back(definition["definition"]
                                                 .toString()
                                                 .replace("ﾠ", " ")
                                                 .toStdString(),
                                             definition["label"]
                                                 .toString()
                                                 .toStdString(),
                                             sentences);
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

std::vector<SourceSentence> parseSentences(QSqlQuery &query)
{
    std::vector<SourceSentence> sentences;

    int simplifiedIndex = query.record().indexOf("simplified");
    int traditionalIndex = query.record().indexOf("traditional");
    int jyutpingIndex = query.record().indexOf("jyutping");
    int pinyinIndex = query.record().indexOf("pinyin");
    int sourceLanguageIndex = query.record().indexOf("language");
    int translationsIndex = query.record().indexOf("translations");
    int definitionSourceNameIndex = query.record().indexOf("sourcename");

    while (query.next()) {
        // Get fields from table
        std::string simplified
            = query.value(simplifiedIndex).toString().toStdString();
        std::string traditional
            = query.value(traditionalIndex).toString().toStdString();
        std::string jyutping
            = query.value(jyutpingIndex).toString().toStdString();
        std::string pinyin = query.value(pinyinIndex).toString().toStdString();
        std::string sourceLanguage
            = query.value(sourceLanguageIndex).toString().toStdString();
        std::string combinedTargetSentencesData
            = query.value(translationsIndex).toString().toStdString();
        std::string definitionSourceName
            = query.value(definitionSourceNameIndex).toString().toStdString();

        // Each sentence will have a vector of SentenceSets that represents
        // a group of translations that are from the same source
        std::vector<SentenceSet> sentence_translation_sets;

        if (!combinedTargetSentencesData.empty()) {
            // Parse JSON returned by query
            QJsonDocument doc = QJsonDocument::fromJson(
                QString::fromStdString(combinedTargetSentencesData).toUtf8());
            // Parse each of the sentence translation groups
            foreach (const QJsonValue &translation_set, doc.array()) {
                std::string sentenceSourceName
                    = translation_set["source"].toString().toStdString();

                // If the sentence has the source it comes from, use it
                // Otherwise, assume it comes from the same source as the
                // definition it is linked to (if any)
                sentence_translation_sets.emplace_back(
                    sentenceSourceName.empty() ? definitionSourceName
                                               : sentenceSourceName);

                foreach (const QJsonValue &translation,
                         translation_set["translations"].toArray()) {
                    // Parse each translation in this group
                    std::vector<Sentence::TargetSentence> targetSentences;
                    sentence_translation_sets.back().pushSentence(
                        {translation["sentence"].toString().toStdString(),
                         translation["language"].toString().toStdString(),
                         translation["direct"].toInt() == 1});
                }
            }
        }

        sentences.emplace_back(sourceLanguage,
                               simplified,
                               traditional,
                               jyutping,
                               pinyin,
                               sentence_translation_sets);
    }

    return sentences;
}

bool parseExistence(QSqlQuery &query)
{
    bool existence = false;

    int existenceIndex = query.record().indexOf("existence");

    while (query.next()) {
        existence = query.value(existenceIndex).toInt() == 1;
    }

    return existence;
}

std::vector<searchTermHistoryItem> parseHistoryItems(QSqlQuery &query)
{
    std::vector<searchTermHistoryItem> results;

    int textIndex = query.record().indexOf("text");
    int optionsIndex = query.record().indexOf("options");

    while (query.next()) {
        std::string text = query.value(textIndex).toString().toStdString();
        int options = query.value(optionsIndex).toInt();

        results.push_back(searchTermHistoryItem{text, options});
    }

    return results;
}

}

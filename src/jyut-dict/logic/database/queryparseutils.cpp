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
            QJsonDocument doc = QJsonDocument::fromJson(QString{definition.c_str()}.toUtf8());
            // Each object in the array represents a group of definitions
            // that are all from the same source
            for (QJsonValue definitionGroup : doc.array()) {

                std::string sourceName = definitionGroup["source"].toString().toStdString();
                std::vector<Definition::Definition> definitions;

                for (QJsonValue definition : definitionGroup["definitions"].toArray()) {

                    std::vector<SourceSentence> sentences;
                    for (QJsonValue sentence : definition["sentences"].toArray()) {
                        std::vector<SentenceSet> sentence_translations;

                        // Parse each sentence
                        if (!sentence.isNull()) {
                            std::vector<Sentence::TargetSentence> targetSentences;
                            if (!sentence["translations"].isNull()) {
                                // Parse each of the sentence translations
                                for (QJsonValue translation : sentence["translations"].toArray()) {
                                    targetSentences.emplace_back(
                                        translation["sentence"]
                                            .toString()
                                            .toStdString(),
                                        translation["language"]
                                            .toString()
                                            .toStdString(),
                                        translation["direct"].toBool());
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

                    definitions.emplace_back(definition["definition"].toString().toStdString(),
                                             definition["label"].toString().toStdString(),
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

    int sourceSentenceIdIndex = query.record().indexOf("chinese_sentence_id");
    int simplifiedIndex = query.record().indexOf("simplified");
    int traditionalIndex = query.record().indexOf("traditional");
    int jyutpingIndex = query.record().indexOf("jyutping");
    int pinyinIndex = query.record().indexOf("pinyin");
    int sourceLanguageIndex = query.record().indexOf("source_language");
    int sentencesIndex = query.record().indexOf("sentences");

    while (query.next()) {
        // Get fields from table
        std::string sourceSentenceId
            = query.value(sourceSentenceIdIndex).toString().toStdString();
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
            = query.value(sentencesIndex).toString().toStdString();
        if (combinedTargetSentencesData.empty()) {
            continue;
        }

        // Parse sentences
        std::vector<std::string> targetSentencesData;
        Utils::split(combinedTargetSentencesData, "●", targetSentencesData);

        // Put target sentences in the correct SentenceSet
        std::vector<SentenceSet> sentenceSets = {};
        for (std::string &targetSentenceData : targetSentencesData) {
            SentenceSet *set;

            // Currently, layout of sentences column is
            // sourcename targetlanguage direct targetsentencecontent
            // (with spaces separating those)
            std::string::size_type first_space_index = targetSentenceData
                                                           .find_first_of(" ");
            std::string::size_type second_space_index
                = targetSentenceData.find(" ", first_space_index + 1);
            std::string::size_type third_space_index
                = targetSentenceData.find(" ", second_space_index + 1);

            std::string source = targetSentenceData.substr(0, first_space_index);
            std::string targetLanguage = targetSentenceData
                                             .substr(first_space_index + 1,
                                                     second_space_index
                                                         - first_space_index);
            std::string direct = targetSentenceData
                                     .substr(second_space_index + 1,
                                             third_space_index
                                                 - second_space_index);
            std::string targetSentenceContent = targetSentenceData.substr(
                third_space_index + 1);

            // Search sentenceSets for a matching set (i.e. set with same source)
            // Create a new sentenceSet for set if it no matches found
            // Then get a handle for that set
            auto search = std::find_if(sentenceSets.begin(),
                                       sentenceSets.end(),
                                       [source](const SentenceSet &set) {
                                           return set.getSource() == source;
                                       });
            if (search == sentenceSets.end()) {
                sentenceSets.push_back(SentenceSet{source});
                set = &sentenceSets.back();
            } else {
                set = &*search;
            }

            // Push the sentence to that set
            Sentence::TargetSentence targetSentence = {targetSentenceContent,
                                                       targetLanguage,
                                                       direct == "1"};
            set->pushSentence(targetSentence);
        }

        sentences.push_back(SourceSentence{sourceLanguage,
                                           simplified,
                                           traditional,
                                           jyutping,
                                           pinyin,
                                           sentenceSets});
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

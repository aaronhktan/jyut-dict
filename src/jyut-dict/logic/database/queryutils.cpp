#include "queryutils.h"

#include "logic/utils/utils.h"

#include <QSqlRecord>

namespace QueryUtils {

std::vector<Entry> parseEntries(QSqlQuery &query)
{
    std::vector<Entry> entries;

    int simplifiedIndex = query.record().indexOf("simplified");
    int traditionalIndex = query.record().indexOf("traditional");
    int jyutpingIndex = query.record().indexOf("jyutping");
    int pinyinIndex = query.record().indexOf("pinyin");
    int definitionIndex = query.record().indexOf("definitions");

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

        // Parse definitions
        std::vector<std::string> definitions;
        Utils::split(definition, "●", definitions);

        // Put definitions in the correct DefinitionsSet
        std::vector<DefinitionsSet> definitionsSets = {};
        for (std::string &definition : definitions) {
            DefinitionsSet *set;
            std::string source = definition.substr(0,
                                                   definition.find_first_of(
                                                       " "));

            // Search definitionsSets for a matching set (i.e. set with same source)
            // Create a new DefinitionsSet for set if it no matches found
            // Then get a handle on that set
            auto search = std::find_if(definitionsSets.begin(),
                                       definitionsSets.end(),
                                       [source,
                                        definition](const DefinitionsSet &set) {
                                           return set.getSource() == source;
                                       });
            if (search == definitionsSets.end()) {
                definitionsSets.push_back(DefinitionsSet{source});
                set = &definitionsSets.back();
            } else {
                set = &*search;
            }

            // Push the definition to that set
            std::string definitionContent = definition.substr(
                definition.find_first_of(" ") + 1);

            set->pushDefinition(definitionContent);
        }

        entries.push_back(Entry(simplified,
                                traditional,
                                jyutping,
                                pinyin,
                                definitionsSets,
                                std::vector<std::string>{},
                                std::vector<SourceSentence>{}));
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
        for (std::string targetSentenceData : targetSentencesData) {
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
            Sentence::TargetSentence targetSentence = {targetLanguage,
                                                       targetSentenceContent,
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

}

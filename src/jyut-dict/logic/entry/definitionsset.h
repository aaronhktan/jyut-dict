#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include "logic/entry/entryphoneticoptions.h"
#include "logic/dictionary/dictionarysource.h"
#include "logic/sentence/sourcesentence.h"

#include <iostream>
#include <unordered_map>
#include <vector>

namespace Definition {

// The Definition struct provides:
// the definition itself, as well as
// a vector of SourceSentences that shows how it is used in context
struct Definition
{
    std::string definitionContent;
    std::string label;
    std::vector<SourceSentence> sentences;

    Definition(std::string definitionContent, std::string label, std::vector<SourceSentence> sentences):
        definitionContent{definitionContent},
        label{label},
        sentences(sentences)
    {}

    bool operator==(const Definition &other) const
    {
        return definitionContent == other.definitionContent
               && label == other.label && sentences == other.sentences;
    }
};

}

// The DefinitionsSet class contains a grouping of definitions from one source

class DefinitionsSet
{
public:
    DefinitionsSet() = delete;
    DefinitionsSet(std::string source, std::vector<Definition::Definition> definitions);

    friend std::ostream &operator<<(std::ostream &out,
                                    DefinitionsSet const &definitions);
    bool operator==(const DefinitionsSet &other) const
    {
        return _source == other._source && _definitions == other._definitions;
    }

    bool isEmpty() const;

    void pushDefinition(const Definition::Definition definition);
    void generatePhonetic(CantoneseOptions cantoneseOptions,
                          MandarinOptions mandarinOptions);

    std::string getSource() const;
    std::string getSourceLongString() const;
    std::string getSourceShortString() const;
    std::string getDefinitionsSnippet() const;
    std::vector<Definition::Definition> getDefinitions(void) const;

private:
    std::string _source;
    std::vector<Definition::Definition> _definitions;
};

#endif // DEFINITIONS_H

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

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
};

}

// The DefinitionsSet class contains a grouping of definitions from one source

class DefinitionsSet
{
public:
    DefinitionsSet();
    DefinitionsSet(std::string source);
    DefinitionsSet(std::string source, std::vector<Definition::Definition> definitions);

    friend std::ostream &operator<<(std::ostream &out,
                                    DefinitionsSet const &definitions);

    bool isEmpty() const;

    void pushDefinition(const Definition::Definition definition);

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

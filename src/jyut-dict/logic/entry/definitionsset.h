#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include "logic/dictionary/dictionarysource.h"

#include <iostream>
#include <unordered_map>
#include <vector>

// The DefintitionsSet class contains a grouping of definitions from one source

class DefinitionsSet
{
public:
    DefinitionsSet();
    DefinitionsSet(std::string source);
    DefinitionsSet(std::string source, std::vector<std::string> definitions);
    DefinitionsSet(std::string source, std::string definitions);
    DefinitionsSet(const DefinitionsSet &definitions);
    DefinitionsSet(const DefinitionsSet &&definitions);

    DefinitionsSet &operator=(const DefinitionsSet &definitions);
    DefinitionsSet &operator=(const DefinitionsSet &&definitions);
    friend std::ostream &operator<<(std::ostream &out,
                                    DefinitionsSet const &definitions);

    bool isEmpty() const;

    void pushDefinition(const std::string definition);

    std::string getSource() const;
    std::string getSourceLongString() const;
    std::string getSourceShortString() const;
    std::string getDefinitionsSnippet() const;
    std::vector<std::string> getDefinitions(void) const;

private:
    std::vector<std::string> parseDefinitions(std::string definitions);

    std::string _source;
    std::vector<std::string> _definitions;
};

#endif // DEFINITIONS_H

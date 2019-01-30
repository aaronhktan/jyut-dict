#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <iostream>
#include <vector>

// DictionarySource is an enum that contains possible dictionary sources

enum DictionarySource {
    CEDICT,
    CCCANTO
};

// The DefintitionsSet class contains a grouping of definitions from one source

class DefinitionsSet
{
public:
    DefinitionsSet();
    DefinitionsSet(DictionarySource source, std::vector<std::string> definitions);
    DefinitionsSet(DictionarySource source, std::string definitions);
    DefinitionsSet(const DefinitionsSet& definitions);
    DefinitionsSet(const DefinitionsSet&& definitions);

    DefinitionsSet& operator=(const DefinitionsSet& definitions);
    DefinitionsSet& operator=(const DefinitionsSet&& definitions);
    friend std::ostream& operator<<(std::ostream& out, DefinitionsSet const& definitions);

    bool isEmpty() const;

    DictionarySource getSource() const;
    std::string getSourceLongString() const;
    std::string getSourceShortString() const;
    std::string getDefinitionsSnippet() const;
    std::vector<std::string> getDefinitions(void) const;

private:
    std::vector<std::string> parseDefinitions(std::string definitions);

    DictionarySource _source;
    std::vector<std::string> _definitions;
};

#endif // DEFINITIONS_H

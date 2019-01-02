#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <iostream>
#include <vector>

enum DictionarySource {
    CEDICT,
    CCCANTO
};

class DefinitionsSet
{
public:
    DefinitionsSet();
    DefinitionsSet(DictionarySource source, std::vector<std::string> definitions);
    DefinitionsSet(const DefinitionsSet& definitions);
    DefinitionsSet(const DefinitionsSet&& definitions);

    DefinitionsSet& operator=(const DefinitionsSet& definitions);
    DefinitionsSet& operator=(const DefinitionsSet&& definitions);
    friend std::ostream& operator<<(std::ostream& out, const DefinitionsSet& definitions);

    DictionarySource getSource() const;
    std::vector<std::string> getDefinitions(void) const;

private:
    DictionarySource _source;
    std::vector<std::string> _definitions;
};

#endif // DEFINITIONS_H

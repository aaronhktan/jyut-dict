#include "definitionsset.h"

#include <sstream>

DefinitionsSet::DefinitionsSet()
{

}

DefinitionsSet::DefinitionsSet(DictionarySource source, std::vector<std::string> definitions)
    : _source{source},
      _definitions{definitions}
{

}

DefinitionsSet::DefinitionsSet(DictionarySource source, std::string definitions)
    : _source{source}
{
    _definitions = parseDefinitions(definitions);
}

DefinitionsSet::DefinitionsSet(const DefinitionsSet& definitions)
    : _source{definitions.getSource()},
      _definitions{definitions.getDefinitions()}
{

}

DefinitionsSet::DefinitionsSet(const DefinitionsSet&& definitions)
    : _source{definitions.getSource()},
      _definitions{definitions.getDefinitions()}
{

}

DefinitionsSet& DefinitionsSet::operator=(const DefinitionsSet& definitions)
{
    if (&definitions == this) {
        return *this;
    }

    _source = definitions.getSource();
    _definitions = definitions.getDefinitions();

    return *this;
}

DefinitionsSet& DefinitionsSet::operator=(const DefinitionsSet&& definitions)
{
    if (&definitions == this) {
        return *this;
    }

    _source = definitions.getSource();
    _definitions = definitions.getDefinitions();

    return *this;
}

std::ostream& operator<<(std::ostream& out, DefinitionsSet const& definitions)
{
//    out << "Definitions (" << definitions.getSourceLongString() << "):\n";

    for (std::string definition : definitions.getDefinitions()) {
        out << definition << "\n";
    }

    return out;
}

bool DefinitionsSet::isEmpty() const
{
    return _definitions.empty();
}

DictionarySource DefinitionsSet::getSource() const
{
    return _source;
}

std::string DefinitionsSet::getSourceLongString() const
{
    switch (_source) {
        case CEDICT:
            return "CEDICT";
        case CCCANTO:
            return "CC-CANTO";
    }

    return "";
}

std::string DefinitionsSet::getSourceShortString() const
{
    switch (_source) {
        case CEDICT:
            return "CC";
        case CCCANTO:
            return "CCY";
    }

    return "";
}

// getDefinitionSnippet() returns a string that shows the definitions contained
// in a DefinitionsSet object, in one line, separated by semicolons.
// This is useful for the search results list view, where the snippet
// essentially functions as a "preview" of the definitions for a word.
std::string DefinitionsSet::getDefinitionsSnippet() const
{
    if (isEmpty()) {
        return "";
    }

    std::ostringstream definitions;
    for (size_t i = 0; i < _definitions.size() - 1; i++) {
        definitions << _definitions[i] << "; ";
    }
    definitions << _definitions.back();
    return definitions.str();
}

std::vector<std::string> DefinitionsSet::getDefinitions() const
{
    return _definitions;
}

// parseDefinitions takes a string and extracts definitions based on the CEDICT
// standard.
//
// In the standard, each definition is separated by a "/" character.
std::vector<std::string> DefinitionsSet::parseDefinitions(std::string definitions)
{
    std::vector<std::string> definitionsSet;
    std::stringstream ss(definitions);
    std::string definition;

    while (std::getline(ss, definition, '/')) {
        definitionsSet.push_back(definition);
    }

    return definitionsSet;
}

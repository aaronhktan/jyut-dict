#include "definitionsset.h"

DefinitionsSet::DefinitionsSet()
{

}

DefinitionsSet::DefinitionsSet(DictionarySource source, std::vector<std::string> definitions)
    : _source{source},
      _definitions{definitions}
{

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

std::ostream& operator<<(std::ostream& out, DefinitionsSet& definitions)
{
    out << "Source: " << definitions.getSource() << "; ";
    out << "Definitions:\n";

    for (auto definition : definitions.getDefinitions()) {
        out << definition << "\n";
    }

    return out;
}

DictionarySource DefinitionsSet::getSource() const
{
    return _source;
}

std::vector<std::string> DefinitionsSet::getDefinitions() const
{
    return _definitions;
}

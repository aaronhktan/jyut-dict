#include "definitionsset.h"

#include <sstream>

DefinitionsSet::DefinitionsSet(std::string source,
                               std::vector<Definition::Definition> definitions)
    : _source{source},
      _definitions{definitions}
{

}

std::ostream &operator<<(std::ostream &out, DefinitionsSet const &definitions)
{
    out << "= source\n" << definitions.getSource() << "\n";
    for (const auto &definition : definitions.getDefinitions()) {
        out << "== definition\n" << definition.definitionContent << "\n";
        out << "== label\n" << definition.label << "\n";
        for (const auto &e : definition.sentences) {
            out << "===== sentence\n" << e << "\n";
        }
    }

    return out;
}

bool DefinitionsSet::isEmpty() const
{
    return _definitions.empty();
}

void DefinitionsSet::pushDefinition(const Definition::Definition definition)
{
    _definitions.push_back(definition);
}

void DefinitionsSet::generatePhonetic(CantoneseOptions cantoneseOptions,
                                      MandarinOptions mandarinOptions)
{
    for (auto &definition : _definitions) {
        for (auto &sentence : definition.sentences) {
            sentence.generatePhonetic(cantoneseOptions, mandarinOptions);
        }
    }
}

std::string DefinitionsSet::getSource() const
{
    return _source;
}

std::string DefinitionsSet::getSourceLongString() const
{
    return _source;
}

std::string DefinitionsSet::getSourceShortString() const
{
    return DictionarySourceUtils::getSourceShortString(_source);
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
        auto location = _definitions[i].definitionContent.find_first_of("\r\n");
        definitions << _definitions[i].definitionContent.substr(0, location) << "; ";
    }
    auto location = _definitions.back().definitionContent.find_first_of("\r\n");
    definitions << _definitions.back().definitionContent.substr(0, location);
    return definitions.str();
}

std::vector<Definition::Definition> DefinitionsSet::getDefinitions() const
{
    return _definitions;
}

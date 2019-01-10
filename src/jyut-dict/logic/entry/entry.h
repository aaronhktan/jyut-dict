#ifndef ENTRY_H
#define ENTRY_H

#include <logic/entry/definitionsset.h>
#include <logic/entry/sentence.h>

#include <QVariant>

#include <string>
#include <vector>

class Entry : public QObject
{
public:
    Entry();
    Entry(std::string simplified, std::string traditional,
          std::string jyutping, std::string pinyin,
          std::vector<DefinitionsSet> definitions,
          std::vector<std::string> derivedWords,
          std::vector<Sentence> sentences);
    Entry(const Entry& entry);
    Entry(const Entry&& entry);

    Entry& operator=(Entry& entry);
    Entry& operator=(Entry&& entry);
    friend std::ostream& operator<<(std::ostream& out, const Entry& entry);

    std::string getSimplified(void) const;
    void setSimplified(std::string simplified);

    std::string getTraditional(void) const;
    void setTraditional(std::string traditional);

    std::string getJyutping(void) const;
    void setJyutping(std::string jyutping);

    std::string getPinyin(void) const;
    void setPinyin(std::string pinyin);

    std::vector<DefinitionsSet> getDefinitionsSets(void) const;
    std::string getDefinitionSnippet(void) const;
    void addDefinitions(DictionarySource source, std::vector<std::string> definitions);

    std::vector<std::string> getDerivedWords(void) const;
    void setDerivedWords(std::vector<std::string> derivedWords);

    std::vector<Sentence> getSentences(void) const;
    void setSentences(std::vector<Sentence> sentences);

    ~Entry();

private:
    std::string _simplified;
    std::string _traditional;
    std::string _jyutping;
    std::string _pinyin;
    std::vector<DefinitionsSet> _definitions;
    std::vector<std::string> _derivedWords;
    std::vector<Sentence> _sentences;
};

Q_DECLARE_METATYPE(Entry);

#endif // ENTRY_H

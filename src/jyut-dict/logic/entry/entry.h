#ifndef ENTRY_H
#define ENTRY_H

#include <logic/entry/sentence.h>

#include <string>
#include <vector>

class Entry
{
public:
    Entry();
    Entry(std::string word, std::string jyutping, std::string pinyin,
          std::vector<std::string> definitions,
          std::vector<std::string> derivedWords,
          std::vector<Sentence> sentences);
    Entry(const Entry& entry);
    Entry(const Entry&& entry);

    Entry& operator=(Entry& entry);
    Entry& operator=(Entry&& entry);
    friend std::ostream& operator<<(std::ostream& out, const Entry& entry);

    std::string getWord(void) const;
    void setWord(std::string word);

    std::string getJyutping(void) const;
    void setJyutping(std::string jyutping);

    std::string getPinyin(void) const;
    void setPinyin(std::string pinyin);

    std::vector<std::string> getDefinitions(void) const;
    void setDefinitions(std::vector<std::string> definitions);

    std::vector<std::string> getDerivedWords(void) const;
    void setDerivedWords(std::vector<std::string> derivedWords);

    std::vector<Sentence> getSentences(void) const;
    void setSentences(std::vector<Sentence> sentences);

    ~Entry();

private:
    std::string _word;
    std::string _jyutping;
    std::string _pinyin;
    std::vector<std::string> _definitions;
    std::vector<std::string> _derivedWords;
    std::vector<Sentence> _sentences;
};

#endif // ENTRY_H

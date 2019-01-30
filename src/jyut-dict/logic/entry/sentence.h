#ifndef SENTENCE_H
#define SENTENCE_H

#include <string>

// The Sentence class will in the future contain groupings of translations
// of a sentence

class Sentence
{
public:
    Sentence();
    Sentence(std::string cantonese, std::string english, std::string mandarin);
    Sentence(const Sentence& sentence);
    Sentence(const Sentence&& sentence);

    Sentence& operator=(const Sentence& sentence);
    Sentence& operator=(const Sentence&& sentence);
    friend std::ostream& operator<<(std::ostream& out, const Sentence& sentence);

    ~Sentence();

    std::string getCantonese(void) const;
    void setCantonese(std::string cantonese);
    std::string getEnglish(void) const;
    void setEnglish(std::string english);
    std::string getMandarin(void) const;
    void setMandarin(std::string mandarin);

private:
    std::string _cantonese;
    std::string _english;
    std::string _mandarin;
};

#endif // SENTENCE_H

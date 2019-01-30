#include "sentence.h"

#include <iostream>

Sentence::Sentence()
{
}

Sentence::Sentence(std::string cantonese, std::string english, std::string mandarin)
    : _cantonese{cantonese},
      _english{english},
      _mandarin{mandarin}
{
}

Sentence::Sentence(const Sentence& sentence)
    : _cantonese{sentence.getCantonese()},
      _english{sentence.getEnglish()},
      _mandarin{sentence.getMandarin()}
{
}

Sentence::Sentence(const Sentence&& sentence)
    : _cantonese{std::move(sentence._cantonese)},
      _english{std::move(sentence._english)},
      _mandarin{sentence._mandarin}
{
}

Sentence& Sentence::operator=(const Sentence& sentence)
{
    if (&sentence == this) {
        return *this;
    }

    _cantonese = sentence.getCantonese();
    _english = sentence.getEnglish();
    _mandarin = sentence.getMandarin();

    return *this;
}

Sentence& Sentence::operator=(const Sentence&& sentence)
{
    if (&sentence == this) {
        return *this;
    }

    _cantonese = sentence.getCantonese();
    _english = sentence.getEnglish();
    _mandarin = sentence.getMandarin();

    return *this;
}

std::ostream& operator<<(std::ostream& out, const Sentence& sentence)
{
    out << "Cantonese: " << sentence.getCantonese() << "; ";
    out << "English: " << sentence.getEnglish() << "; ";
    out << "Mandarin: " << sentence.getMandarin() << "; ";
    return out;
}

std::string Sentence::getCantonese(void) const
{
    return _cantonese;
}

void Sentence::setCantonese(std::string cantonese)
{
    _cantonese = cantonese;
}

std::string Sentence::getEnglish(void) const
{
    return _english;
}

void Sentence::setEnglish(std::string english)
{
    _english = english;
}

std::string Sentence::getMandarin(void) const
{
    return _mandarin;
}

void Sentence::setMandarin(std::string mandarin)
{
    _mandarin = mandarin;
}

Sentence::~Sentence()
{

}

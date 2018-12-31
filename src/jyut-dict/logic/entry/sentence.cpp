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
    std::cout << "Using normal constructor" << std::endl;
}

Sentence::Sentence(const Sentence& sentence)
    : _cantonese{sentence.getCantonese()},
      _english{sentence.getEnglish()},
      _mandarin{sentence.getMandarin()}
{
    std::cout << "Using copy constructor" << std::endl;
}

Sentence::Sentence(const Sentence&& sentence)
    : _cantonese{sentence.getCantonese()},
      _english{sentence.getEnglish()},
      _mandarin{sentence.getMandarin()}
{
    std::cout << "Using move constructor" << std::endl;
}

Sentence& Sentence::operator=(const Sentence& sentence)
{
    if (&sentence == this) {
        return *this;
    }

    _cantonese = sentence.getCantonese();
    _english = sentence.getEnglish();
    _mandarin = sentence.getMandarin();

    std::cout << "Using copy operator" << std::endl;
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

    std::cout << "Using move operator" << std::endl;
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

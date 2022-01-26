#ifndef ICARDHEADER_H
#define ICARDHEADER_H

#include <string>

// Interface for header widgets, displaying the source of definitions

class ICardHeaderWidget
{
public:
    virtual void setSectionTitle(const std::string &name) = 0;

    virtual ~ICardHeaderWidget() = default;
};

#endif // ICARDHEADER_H

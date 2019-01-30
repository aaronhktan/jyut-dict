#ifndef ISECTIONHEADER_H
#define ISECTIONHEADER_H

#include <string>

// Interface for header widgets, displaying the source of definitions

class ISectionHeaderWidget
{
public:
    virtual void setSectionTitle(std::string name) = 0;

    virtual ~ISectionHeaderWidget() {}
};

#endif // ISECTIONHEADER_H

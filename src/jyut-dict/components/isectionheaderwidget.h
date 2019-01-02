#ifndef ISECTIONHEADER_H
#define ISECTIONHEADER_H

#include <string>

class ISectionHeaderWidget
{
public:
    virtual void setSectionTitle(std::string name) = 0;

    virtual ~ISectionHeaderWidget() {};
};

#endif // ISECTIONHEADER_H

//
//  Zinnia: Online hand recognition system with machine learning
//
//  $Id: stream_wrapper.h 17 2009-04-05 11:40:32Z taku-ku $;
//
//  Copyright(C) 2008 Taku Kudo <taku@chasen.org>
//
//
//  CRF++ -- Yet Another CRF toolkit
//
//  $Id: stream_wrapper.h 17 2009-04-05 11:40:32Z taku-ku $;
//
//  Copyright(C) 2005-2007 Taku Kudo <taku@chasen.org>
//
#ifndef zinnia_STREAM_WRAPPER_H_
#define zinnia_STREAM_WRAPPER_H_

#include <cstring>
#include <fstream>
#include <iostream>

namespace zinnia {

class istream_wrapper
{
private:
    std::istream *is;

public:
    std::istream &operator*() const { return *is; }
    std::istream *operator->() const { return is; }
    std::istream *get() { return is; }
    explicit istream_wrapper(const char *filename)
        : is(0)
    {
        if (std::strcmp(filename, "-") == 0)
            is = &std::cin;
        else
            is = new std::ifstream(filename);
    }

    ~istream_wrapper()
    {
        if (is != &std::cin)
            delete is;
    }
};

class ostream_wrapper
{
private:
    std::ostream *os;

public:
    std::ostream &operator*() const { return *os; }
    std::ostream *operator->() const { return os; }
    std::ostream *get() { return os; }
    explicit ostream_wrapper(const char *filename)
        : os(0)
    {
        if (std::strcmp(filename, "-") == 0)
            os = &std::cout;
        else
            os = new std::ofstream(filename);
    }

    ~ostream_wrapper()
    {
        if (os != &std::cout)
            delete os;
    }
};
} // namespace zinnia

#endif

//
//  Zinnia: Online hand recognition system with machine learning
//
//  $Id: common.h 24 2009-05-31 04:32:53Z taku-ku $;
//
//  Copyright(C) 2008 Taku Kudo <taku@chasen.org>
//
#ifndef zinnia_COMMON_H_
#define zinnia_COMMON_H_

#include <cstdlib>
#include <iostream>
#include <setjmp.h>
#include <sstream>
#include <string>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define COPYRIGHT \
    "zinnia: Yet Another Hand Written Character Recognizer\nCopyright(C)" \
    " 2005-2009 Taku Kudo, All rights reserved.\n"

// tricky macro for MSVC
#ifdef _MSC_VER
#define for \
    if (0) \
        ; \
    else for
/* why windows.h define such a generic macro */
#undef max
#undef min
#endif

#define BUF_SIZE (8192)

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#define DIC_VERSION (1)
#define DIC_MAGIC_ID (0xef71821u)

namespace zinnia {

template<class T>
inline T _min(T x, T y)
{
    return (x < y) ? x : y;
}
template<class T>
inline T _max(T x, T y)
{
    return (x > y) ? x : y;
}

class die
{
public:
    die() {}
    ~die()
    {
        std::cerr << std::endl;
        exit(-1);
    }
    int operator&(std::ostream &) { return 0; }
};

class warn
{
public:
    warn() {}
    ~warn() { std::cerr << std::endl; }
    int operator&(std::ostream &) { return 0; }
};

struct whatlog
{
    std::ostringstream stream_;
    std::string str_;
    const char *str()
    {
        str_ = stream_.str();
        return str_.c_str();
    }
    jmp_buf cond_;
};

class wlog
{
public:
    whatlog *l_;
    explicit wlog(whatlog *l)
        : l_(l)
    {
        l_->stream_.clear();
    }
    ~wlog() { longjmp(l_->cond_, 1); }
    int operator&(std::ostream &) { return 0; }
};
} // namespace zinnia

#define WHAT what_.stream_

#define CHECK_RETURN(condition, value) \
    if (condition) { \
    } else if (setjmp(what_.cond_) == 1) { \
        return value; \
    } else \
        wlog(&what_) \
            & what_.stream_ << __FILE__ << "(" << __LINE__ << ") [" \
                            << #condition << "] "

#define CHECK_0(condition) CHECK_RETURN(condition, 0)
#define CHECK_FALSE(condition) CHECK_RETURN(condition, false)

#define CHECK_CLOSE_FALSE(condition) \
    if (condition) { \
    } else if (setjmp(what_.cond_) == 1) { \
        close(); \
        return false; \
    } else \
        wlog(&what_) \
            & what_.stream_ << __FILE__ << "(" << __LINE__ << ") [" \
                            << #condition << "] "

#define CHECK_DIE(condition) \
    (condition) ? 0 \
                : die() \
                      & std::cerr << __FILE__ << "(" << __LINE__ << ") [" \
                                  << #condition << "] "

#define CHECK_WARN(condition) \
    (condition) ? 0 \
                : warn() \
                      & std::cerr << __FILE__ << "(" << __LINE__ << ") [" \
                                  << #condition << "] "
#endif

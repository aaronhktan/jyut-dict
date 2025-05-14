//
//  Zinnia: Online hand recognition system with machine learning
//
//  $Id: freelist.h 17 2009-04-05 11:40:32Z taku-ku $;
//
//  Copyright(C) 2008 Taku Kudo <taku@chasen.org>
//
#ifndef CRFPP_FREELIST_H_
#define CRFPP_FREELIST_H_

#include <cstring>
#include <vector>

namespace zinnia {
template<class T>
class Length
{
public:
    size_t operator()(const T *str) const { return 1; }
};

class charLength
{
public:
    size_t operator()(const char *str) const { return strlen(str) + 1; }
};

template<class T, class LengthFunc = Length<T>>
class FreeList
{
private:
    std::vector<T *> freeList;
    size_t pi;
    size_t li;
    size_t size;

public:
    void free() { li = pi = 0; }

    T *alloc(size_t len = 1)
    {
        if ((pi + len) >= size) {
            li++;
            pi = 0;
        }
        if (li == freeList.size()) {
            freeList.push_back(new T[size]);
        }
        T *r = freeList[li] + pi;
        pi += len;
        return r;
    }

    T *dup(T *src, size_t len = 0)
    {
        if (!len)
            len = LengthFunc()(src);
        T *p = alloc(len);
        if (src == 0)
            memset(p, 0, len * sizeof(T));
        else
            memcpy(p, src, len * sizeof(T));
        return p;
    }

    void set_size(size_t n) { size = n; }

    explicit FreeList(size_t _size)
        : pi(0)
        , li(0)
        , size(_size)
    {}
    explicit FreeList()
        : pi(0)
        , li(0)
        , size(0)
    {}

    virtual ~FreeList()
    {
        for (li = 0; li < freeList.size(); ++li) {
            delete[] freeList[li];
        }
    }
};

typedef FreeList<char, charLength> StrFreeList;
} // namespace zinnia
#endif

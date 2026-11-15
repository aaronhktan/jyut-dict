//
//  Zinnia: Online hand recognition system with machine learning
//
//  $Id: scoped_ptr.h 17 2009-04-05 11:40:32Z taku-ku $;
//
//  Copyright(C) 2008 Taku Kudo <taku@chasen.org>
//
#ifndef zinnia_SCOPED_PTR_H_
#define zinnia_SCOPED_PTR_H_

#include <cstring>
#include <string>

namespace zinnia {

template<class T>
class scoped_ptr
{
private:
    T *ptr_;
    scoped_ptr(scoped_ptr const &);
    scoped_ptr &operator=(scoped_ptr const &);
    typedef scoped_ptr<T> this_type;

public:
    typedef T element_type;
    explicit scoped_ptr(T *p = 0)
        : ptr_(p)
    {}
    virtual ~scoped_ptr() { delete ptr_; }
    void reset(T *p = 0)
    {
        delete ptr_;
        ptr_ = p;
    }
    T &operator*() const { return *ptr_; }
    T *operator->() const { return ptr_; }
    T *get() const { return ptr_; }
};

template<class T>
class scoped_array
{
private:
    T *ptr_;
    scoped_array(scoped_array const &);
    scoped_array &operator=(scoped_array const &);
    typedef scoped_array<T> this_type;

public:
    typedef T element_type;
    explicit scoped_array(T *p = 0)
        : ptr_(p)
    {}
    virtual ~scoped_array() { delete[] ptr_; }
    void reset(T *p = 0)
    {
        delete[] ptr_;
        ptr_ = p;
    }
    T &operator*() const { return *ptr_; }
    T *operator->() const { return ptr_; }
    T *get() const { return ptr_; }
    T &operator[](size_t i) const { return ptr_[i]; }
};

class scoped_string : public scoped_array<char>
{
public:
    explicit scoped_string() { reset_string(""); }
    explicit scoped_string(const std::string &str) { reset_string(str); }

    void reset_string(const std::string &str)
    {
        char *p = new char[str.size() + 1];
        std::strcpy(p, str.c_str());
        reset(p);
    }

    void reset_string(const char *str)
    {
        char *p = new char[std::strlen(str) + 1];
        std::strcpy(p, str);
        reset(p);
    }
};
} // namespace zinnia
#endif

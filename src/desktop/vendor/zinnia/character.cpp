//
//  Zinnia: Online hand recognition system with machine learning
//
//  $Id: character.cpp 17 2009-04-05 11:40:32Z taku-ku $;
//
//  Copyright(C) 2008 Taku Kudo <taku@chasen.org>
//
#include "common.h"
#include "scoped_ptr.h"
#include "sexp.h"
#include "zinnia.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace zinnia {

class CharacterImpl : public Character
{
public:
    void set_value(const char *str) { value_.assign(str, std::strlen(str)); }

    void set_value(const char *str, size_t length)
    {
        value_.assign(str, length);
    }

    const char *value() const { return value_.c_str(); }

    void set_width(size_t width) { width_ = width; }

    void set_height(size_t height) { height_ = height; }

    size_t width() const { return width_; }
    size_t height() const { return height_; }
    void clear() { strokes_.clear(); }
    bool add(size_t id, int x, int y);

    size_t strokes_size() const { return strokes_.size(); }

    size_t stroke_size(size_t id) const
    {
        return (strokes_.size() <= id) ? static_cast<size_t>(-1)
                                       : strokes_[id].size();
    }

    int x(size_t id, size_t i) const
    {
        return (id >= strokes_.size() || i >= strokes_[id].size())
                   ? -1
                   : strokes_[id][i].x;
    }

    int y(size_t id, size_t i) const
    {
        return (id >= strokes_.size() || i >= strokes_[id].size())
                   ? -1
                   : strokes_[id][i].y;
    }

    bool parse(const char *str, size_t length);

    bool parse(const char *str) { return parse(str, std::strlen(str)); }

    bool toString(char *buf, size_t length) const;

    const char *what() { return what_.str(); }

    CharacterImpl()
        : width_(300)
        , height_(300)
    {}
    virtual ~CharacterImpl() { clear(); }

private:
    struct Dot
    {
        int x;
        int y;
    };
    std::vector<std::vector<Dot>> strokes_;
    std::string value_;
    size_t width_;
    size_t height_;
    whatlog what_;
    scoped_ptr<Sexp> sexp_;
};

bool CharacterImpl::add(size_t id, int x, int y)
{
    strokes_.resize(id + 1);
    Dot d;
    d.x = x;
    d.y = y;
    strokes_[id].push_back(d);
    return true;
}

// parse S-expression
// (character
//   (value A)
//   (width 100)
//   (height 100)
//   (strokes ( (0 1) (1 2) ) ( (0 1) (1 2) ) ) )
bool CharacterImpl::parse(const char *str, size_t length)
{
    clear();
    if (!sexp_.get()) {
        sexp_.reset(new Sexp);
    }
    sexp_->free();
    char *begin = const_cast<char *>(str);
    const char *end = str + length;
    const Sexp::Cell *root_cell = sexp_->read(&begin, end);

    if (!root_cell) {
        sexp_->free();
        return false;
    }
    const Sexp::Cell *ccel = root_cell->car();
    if (!ccel->is_atom() || std::strcmp("character", ccel->atom()) != 0) {
        sexp_->free();
        return false;
    }

    for (const Sexp::Cell *it = root_cell->cdr(); it; it = it->cdr()) {
        const Sexp::Cell *cell = it->car();
        if (cell->car() && cell->car()->is_atom() && cell->cdr()
            && cell->cdr()->car() && cell->cdr()->car()->is_atom()) {
            const char *name = cell->car()->atom();
            const char *value = cell->cdr()->car()->atom();
            if (std::strcmp("value", name) == 0) {
                set_value(value);
            } else if (std::strcmp("width", name) == 0) {
                set_width(std::atoi(value));
            } else if (std::strcmp("height", name) == 0) {
                set_height(std::atoi(value));
            }
        }

        if (cell->car() && cell->car()->is_atom() && cell->cdr()
            && cell->cdr()->car() && cell->cdr()->car()->is_cons()
            && std::strcmp("strokes", cell->car()->atom()) == 0) {
            int id = 0;
            for (const Sexp::Cell *st = cell->cdr(); st; st = st->cdr()) {
                for (const Sexp::Cell *dot = st->car(); dot; dot = dot->cdr()) {
                    if (dot->car() && dot->car()->car()
                        && dot->car()->car()->is_atom() && dot->car()->cdr()
                        && dot->car()->cdr()->car()
                        && dot->car()->cdr()->car()->is_atom()) {
                        const int x = std::atoi(dot->car()->car()->atom());
                        const int y = std::atoi(
                            dot->car()->cdr()->car()->atom());
                        add(id, x, y);
                    }
                }
                ++id;
            }
        }
    }

    return true;
}

bool CharacterImpl::toString(char *str, size_t length) const
{
    std::ostringstream os;
    os << "(character (value " << value() << ")";
    os << "(width " << width() << ")";
    os << "(height " << height() << ")";
    os << "(stroeks ";
    for (size_t id = 0; id < strokes_size(); ++id) {
        os << "(";
        for (size_t s = 0; s < stroke_size(id); ++s) {
            os << "(" << x(id, s) << " " << y(id, s) << ")";
        }
        os << ")";
    }
    os << ")";
    const std::string &buf = os.str();
    if (buf.size() < length) {
        memcpy(str, buf.data(), buf.size());
        str[buf.size()] = '\0';
    }
    return true;
}

Character *Character::create()
{
    return new CharacterImpl;
}

Character *createCharacter()
{
    return new CharacterImpl;
}
} // namespace zinnia

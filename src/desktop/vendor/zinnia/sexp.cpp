//
//  Zinnia: Online hand recognition system with machine learning
//
//  $Id: sexp.cpp 17 2009-04-05 11:40:32Z taku-ku $;
//
//  Copyright(C) 2008 Taku Kudo <taku@chasen.org>
//
#include "sexp.h"
#include <iostream>

namespace zinnia {

namespace {
void dump_cdr_internal(const Sexp::Cell *cell, std::ostream *os);

void dump_internal(const Sexp::Cell *cell, std::ostream *os)
{
    if (!cell) {
        *os << "NIL";
    } else {
        if (cell->is_cons()) {
            *os << '(';
            dump_internal(cell->car(), os);
            dump_cdr_internal(cell->cdr(), os);
            *os << ')';
        } else if (cell->is_atom()) {
            *os << cell->atom();
        }
    }
}

void dump_cdr_internal(const Sexp::Cell *cell, std::ostream *os)
{
    if (!cell) {
        return;
    }
    if (cell->is_cons()) {
        *os << ' ';
        dump_internal(cell->car(), os);
        dump_cdr_internal(cell->cdr(), os);
    } else {
        *os << ' ';
        dump_internal(cell, os);
    }
}
} // namespace

const Sexp::Cell *Sexp::read(char **begin, const char *end)
{
    comment(begin, end);
    if (*begin >= end) {
        return 0;
    }
    const int r = next_token(begin, end, '(');
    if (r == 1) {
        return read_car(begin, end);
    } else if (r == 0) {
        return read_atom(begin, end);
    }
    return 0;
}

void Sexp::dump(const Sexp::Cell *cell, std::ostream *os)
{
    dump_internal(cell, os);
    *os << std::endl;
}

void Sexp::free()
{
    cell_freelist_.free();
    char_freelist_.free();
}

int Sexp::next_token(char **begin, const char *end, const char n)
{
    char c = 0;
    do {
        c = **begin;
        ++(*begin);
    } while (isspace(c));

    if (*begin >= end) {
        return -1;
    }

    if (c == n) {
        return 1;
    } else {
        --(*begin);
        return 0;
    }
}

void Sexp::comment(char **begin, const char *end)
{
    const int r = next_token(begin, end, ';');
    if (r == 1) {
        while (*begin < end) {
            const char c = **begin;
            ++(*begin);
            if (c == '\r' || c == '\n') {
                break;
            }
        }
        return comment(begin, end);
    }
}

const Sexp::Cell *Sexp::read_cdr(char **begin, const char *end)
{
    comment(begin, end);
    const int r = next_token(begin, end, ')');
    if (r == 1) {
        return 0;
    } else if (r == 0) {
        return read_car(begin, end);
    }
    return 0;
}

const Sexp::Cell *Sexp::read_car(char **begin, const char *end)
{
    comment(begin, end);
    const int r = next_token(begin, end, ')');
    if (r == 1) {
        return 0;
    } else if (r == 0) {
        Cell *cell = cell_freelist_.alloc(1);
        cell->set_car(this->read(begin, end));
        cell->set_cdr(read_cdr(begin, end));
        return cell;
    }
    return 0;
}

const Sexp::Cell *Sexp::read_atom(char **begin, const char *end)
{
    comment(begin, end);
    char c = **begin;
    const char *p = *begin;
    ++(*begin);
    if (isspace(c) || c == '(' || c == ')') {
        return 0;
    } else if (*begin >= end) {
        return 0;
    } else {
        while (true) {
            c = **begin;
            ++(*begin);
            if (isspace(c) || c == '(' || c == ')' || *begin >= end) {
                --(*begin);
                const size_t length = static_cast<size_t>(*begin - p);
                char *tmp = char_freelist_.alloc(length + 1);
                std::copy(p, p + length, tmp);
                tmp[length] = '\0';
                Cell *cell = cell_freelist_.alloc(1);
                cell->set_atom(tmp);
                return cell;
            }
        }
    }
    return 0;
}
} // namespace zinnia

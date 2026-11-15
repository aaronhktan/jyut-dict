//
//  Zinnia: Online hand recognition system with machine learning
//
//  $Id: trainer.cpp 27 2010-05-09 05:34:05Z taku-ku $;
//
//  Copyright(C) 2008 Taku Kudo <taku@chasen.org>
//
#include "common.h"
#include "feature.h"
#include "mmap.h"
#include "scoped_ptr.h"
#include "svm.h"
#include "zinnia.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

namespace zinnia {

template<class Iterator>
inline size_t tokenize(char *str, const char *del, Iterator out, size_t max)
{
    char *stre = str + std::strlen(str);
    const char *dele = del + std::strlen(del);
    size_t size = 0;

    while (size < max) {
        char *n = std::find_first_of(str, stre, del, dele);
        *n = '\0';
        *out++ = str;
        ++size;
        if (n == stre)
            break;
        str = n + 1;
    }

    return size;
}

FeatureNode *copy_feature_node(const FeatureNode *fn, size_t *max_dim)
{
    size_t d = 0;
    *max_dim = 0;
    CHECK_DIE(fn->index == 0);
    CHECK_DIE(fn->value == 1.0);
    for (const FeatureNode *f = fn; f->index >= 0; ++f) {
        *max_dim = _max(static_cast<size_t>(f->index), *max_dim);
        ++d;
    }
    FeatureNode *x = new FeatureNode[d + 1];

    size_t i = 0;
    for (const FeatureNode *f = fn; f->index >= 0; ++f) {
        x[i].index = f->index;
        x[i].value = f->value;
        ++i;
    }

    x[i].index = -1;
    x[i].value = 0;

    return x;
}

bool make_example(const std::string &key,
                  const std::vector<std::pair<std::string, FeatureNode *>> &x,
                  std::vector<float> *y,
                  std::vector<FeatureNode *> *copy_x)
{
    size_t pos_num = 0;
    size_t neg_num = 0;
    y->clear();
    copy_x->clear();
    for (size_t i = 0; i < x.size(); ++i) {
        if (x[i].first == key) {
            y->push_back(+1.0);
            ++pos_num;
        } else {
            y->push_back(-1.0);
            ++neg_num;
        }
        copy_x->push_back(x[i].second);
    }
    return (pos_num > 0 && neg_num > 0);
}

class TrainerImpl : public Trainer
{
private:
    std::vector<std::pair<std::string, FeatureNode *>> x_;
    size_t max_dim_;
    whatlog what_;

public:
    bool add(const Character &character)
    {
        const std::string y = character.value();
        CHECK_FALSE(!y.empty()) << "input character is empty";
        Features features;
        CHECK_FALSE(features.read(character)) << "cannot read character: " << y;
        size_t d = 0;
        FeatureNode *fn = copy_feature_node(features.get(), &d);
        max_dim_ = _max(d, max_dim_);
        if (!fn) {
            return false;
        }
        x_.emplace_back(y, fn);
        return true;
    }

    void clear()
    {
        for (size_t i = 0; i < x_.size(); ++i)
            delete[] x_[i].second;
        x_.clear();
    }

    bool train(const char *filename);

    const char *what() { return what_.str(); }

    TrainerImpl()
        : max_dim_(0)
    {}
    virtual ~TrainerImpl() { clear(); }
};

bool TrainerImpl::train(const char *filename)
{
    std::string text_filename = filename;
    text_filename += ".txt";
    std::ofstream ofs(text_filename.c_str());
    CHECK_FALSE(ofs) << "permission denied: " << text_filename;

    std::set<std::string> dic_set;
    for (size_t i = 0; i < x_.size(); ++i)
        dic_set.insert(x_[i].first);

    std::vector<std::string> dic;
    std::copy(dic_set.begin(), dic_set.end(), std::back_inserter(dic));

    std::vector<double> w(max_dim_ + 1);
    std::vector<float> y;
    std::vector<FeatureNode *> x_copy;

    for (size_t i = 0; i < dic.size(); ++i) {
        CHECK_FALSE(make_example(dic[i], x_, &y, &x_copy))
            << "cannot make training data";
        std::cout << "learning: (" << i << "/" << dic.size() << ")"
                  << " " << dic[i] << " " << std::flush;

        CHECK_FALSE(svm_train(y.size(),
                              w.size(),
                              &y[0],
                              const_cast<const FeatureNode **>(&x_copy[0]),
                              1.0,
                              &w[0]));

        ofs << dic[i] << " " << w[0]; // w[0] is bias
        static const float kThreshold = 1.0e-3;
        for (size_t i = 1; i < w.size(); ++i) {
            if (std::fabs(w[i]) >= kThreshold) {
                ofs << " ";
                ofs << i << ":" << w[i];
            }
        }
        ofs << std::endl;
    }

    CHECK_FALSE(convert(text_filename.c_str(), filename, 0.0))
        << "cannot convert model from" << text_filename << " to " << filename;

    return true;
}

bool Trainer::makeHeader(const char *text_filename,
                         const char *header_filename,
                         const char *name,
                         double compression_threshold)
{
    bool is_binary = false;
    {
        scoped_ptr<Recognizer> r(Recognizer::create());
        is_binary = r->open(text_filename);
    }

    Mmap<char> mmap;
    std::string binary = text_filename;
    if (!is_binary) {
        binary = std::string(header_filename) + ".tmp";
        if (!convert(text_filename, binary.c_str(), compression_threshold)) {
            return false;
        }
    }

    if (!mmap.open(binary.c_str(), "r")) {
        return false;
    }

    std::ofstream ofs(header_filename);
    ofs << "static const size_t " << name << "_size = " << mmap.file_size()
        << ";" << std::endl;

    const char *begin = mmap.begin();
    const char *end = mmap.end();

#if defined(_WIN32) && !defined(__CYGWIN__)
    ofs << "static const unsigned long long " << name << "_uint64[] = {"
        << std::endl;
    ofs.setf(std::ios::hex, std::ios::basefield); // in hex
    ofs.setf(std::ios::showbase);                 // add 0x

    int num = 0;
    while (begin < end) {
        unsigned long long int n = 0;
        unsigned char *buf = reinterpret_cast<unsigned char *>(&n);
        const size_t size = _min(static_cast<size_t>(end - begin),
                                 static_cast<size_t>(8));
        for (size_t i = 0; i < size; ++i) {
            buf[i] = static_cast<unsigned char>(begin[i]);
        }
        begin += 8;
        ofs << n << ", ";
        if (++num % 8 == 0) {
            ofs << std::endl;
        }
    }
    ofs << "};" << std::endl;
    ofs << "static const char *" << name;
    ofs << " = (const char *)(" << name << "_uint64);" << std::endl;
#else
    ofs << "static const char " << name << "[] =" << std::endl;
    while (begin < end) {
        const int hi = ((static_cast<int>(*begin) & 0xF0) >> 4);
        const int lo = (static_cast<int>(*begin) & 0x0F);
        ofs << "\\x";
        ofs << static_cast<char>(hi >= 10 ? hi - 10 + 'A' : hi + '0');
        ofs << static_cast<char>(lo >= 10 ? lo - 10 + 'A' : lo + '0');
        ++begin;
    }
    ofs << "\";" << std::endl;
#endif

    if (!is_binary) {
        unlink(binary.c_str());
    }
    return true;
}

template<typename T>
inline void write_static(std::ofstream &bofs, const T value)
{
    bofs.write(reinterpret_cast<const char *>(&value), sizeof(T));
}

#ifndef WORDS_LITENDIAN
template<>
inline void write_static<unsigned int>(std::ofstream &bofs,
                                       const unsigned int value)
{
    bofs.put((value) & 0xff);
    bofs.put((value >> 8) & 0xff);
    bofs.put((value >> 16) & 0xff);
    bofs.put((value >> 24) & 0xff);
}

template<>
inline void write_static<float>(std::ofstream &bofs, const float value)
{
    unsigned int x;
    memcpy(&x, &value, sizeof(x));
    write_static<unsigned int>(bofs, x);
}
#endif

bool Trainer::convert(const char *text_filename,
                      const char *binary_filename,
                      double compression_threshold)
{
    std::ifstream ifs(text_filename);
    if (!ifs) {
        std::cerr << "no such file or directory: " << text_filename
                  << std::endl;
        return false;
    }

    std::ofstream bofs(binary_filename, std::ios::binary | std::ios::out);
    if (!bofs) {
        std::cerr << "permission denied: " << binary_filename << std::endl;
        return false;
    }

    unsigned int magic = 0;
    const unsigned int version = DIC_VERSION;
    unsigned int msize = 0;
    write_static<unsigned int>(bofs, magic);
    write_static<unsigned int>(bofs, version);
    write_static<unsigned int>(bofs, msize);

    std::string line;
    const size_t array_size = 8192 * 16;
    scoped_array<char *> col(new char *[array_size]);
    while (std::getline(ifs, line)) {
        char *buf = const_cast<char *>(line.c_str());
        const size_t size = tokenize(buf, " \t:", col.get(), array_size);
        if (size < 5)
            return false;
        if (size % 2 != 0)
            return false;
        const float bias = std::atof(col[1]);
        char character[16];
        std::strncpy(character, col[0], sizeof(character));
        bofs.write(character, sizeof(character));
        write_static<float>(bofs, bias);
        for (size_t i = 2; i < size; i += 2) {
            const int index = std::atoi(col[i]);
            const float value = std::atof(col[i + 1]);
            if (fabs(value) > compression_threshold) {
                write_static<int>(bofs, index);
                write_static<float>(bofs, value);
            }
        }
        const int index = -1;
        const float value = 0.0;
        write_static<int>(bofs, index);
        write_static<float>(bofs, value);
        ++msize;
    }

    magic = static_cast<unsigned int>(bofs.tellp());
    bofs.seekp(0);
    magic ^= DIC_MAGIC_ID;
    bofs.seekp(0);

    write_static<unsigned int>(bofs, magic);
    write_static<unsigned int>(bofs, version);
    write_static<unsigned int>(bofs, msize);

    return true;
}

Trainer *createTrainer()
{
    return new TrainerImpl;
}

Trainer *Trainer::create()
{
    return new TrainerImpl;
}
} // namespace zinnia

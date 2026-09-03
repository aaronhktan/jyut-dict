//
//  Zinnia: Online hand recognition system with machine learning
//
//  $Id: libzinnia.cpp 17 2009-04-05 11:40:32Z taku-ku $;
//
//  Copyright(C) 2008 Taku Kudo <taku@chasen.org>
//
#include "config.h"
#include "param.h"
#include "scoped_ptr.h"
#include "stream_wrapper.h"
#include "zinnia.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <windows.h>
#endif

struct zinnia_character_t
{
    zinnia::Character *ptr;
};

struct zinnia_recognizer_t
{
    zinnia::Recognizer *ptr;
};

struct zinnia_result_t
{
    zinnia::Result *ptr;
};

struct zinnia_trainer_t
{
    zinnia::Trainer *ptr;
};

zinnia_character_t *zinnia_character_new()
{
    zinnia_character_t *character = new zinnia_character_t;
    character->ptr = zinnia::Character::create();
    return character;
}

void zinnia_character_destroy(zinnia_character_t *character)
{
    delete character->ptr;
    delete character;
    character = 0;
}

void zinnia_character_set_value(zinnia_character_t *character, const char *str)
{
    return character->ptr->set_value(str);
}

void zinnia_character_set_value2(zinnia_character_t *character,
                                 const char *str,
                                 size_t length)
{
    return character->ptr->set_value(str, length);
}

const char *zinnia_character_value(zinnia_character_t *character)
{
    return character->ptr->value();
}

void zinnia_character_set_width(zinnia_character_t *character, size_t width)
{
    return character->ptr->set_width(width);
}

void zinnia_character_set_height(zinnia_character_t *character, size_t height)
{
    return character->ptr->set_height(height);
}

size_t zinnia_character_width(zinnia_character_t *character)
{
    return character->ptr->width();
}

size_t zinnia_character_height(zinnia_character_t *character)
{
    return character->ptr->height();
}

void zinnia_character_clear(zinnia_character_t *character)
{
    return character->ptr->clear();
}

int zinnia_character_add(zinnia_character_t *character, size_t id, int x, int y)
{
    return character->ptr->add(id, x, y);
}

size_t zinnia_character_strokes_size(zinnia_character_t *character)
{
    return character->ptr->strokes_size();
}

size_t zinnia_character_stroke_size(zinnia_character_t *character, size_t id)
{
    return character->ptr->stroke_size(id);
}

int zinnia_character_x(zinnia_character_t *character, size_t id, size_t i)
{
    return character->ptr->x(id, i);
}

int zinnia_character_y(zinnia_character_t *character, size_t id, size_t i)
{
    return character->ptr->y(id, i);
}

int zinnia_character_parse(zinnia_character_t *character, const char *str)
{
    return character->ptr->parse(str);
}

int zinnia_character_parse2(zinnia_character_t *character,
                            const char *str,
                            size_t length)
{
    return character->ptr->parse(str, length);
}

int zinnia_character_to_string(zinnia_character_t *character,
                               char *buf,
                               size_t length)
{
    return character->ptr->toString(buf, length);
}

const char *zinnia_character_strerror(zinnia_character_t *character)
{
    return character->ptr->what();
}

const char *zinnia_result_value(zinnia_result_t *result, size_t i)
{
    return result->ptr->value(i);
}

float zinnia_result_score(zinnia_result_t *result, size_t i)
{
    return result->ptr->score(i);
}

size_t zinnia_result_size(zinnia_result_t *result)
{
    return result->ptr->size();
}

void zinnia_result_destroy(zinnia_result_t *result)
{
    delete result->ptr;
    delete result;
    result = 0;
}

zinnia_recognizer_t *zinnia_recognizer_new()
{
    zinnia_recognizer_t *recognizer = new zinnia_recognizer_t;
    recognizer->ptr = zinnia::Recognizer::create();
    return recognizer;
}

void zinnia_recognizer_destroy(zinnia_recognizer_t *recognizer)
{
    delete recognizer->ptr;
    delete recognizer;
    recognizer = 0;
}

int zinnia_recognizer_open(zinnia_recognizer_t *recognizer, const char *filename)
{
    return recognizer->ptr->open(filename);
}

int zinnia_recognizer_open_from_ptr(zinnia_recognizer_t *recognizer,
                                    const char *ptr,
                                    size_t size)
{
    return recognizer->ptr->open(ptr, size);
}

int zinnia_recognizer_close(zinnia_recognizer_t *recognizer)
{
    return recognizer->ptr->close();
}

size_t zinnia_recognizer_size(zinnia_recognizer_t *recognizer)
{
    return recognizer->ptr->size();
}

const char *zinnia_recognizer_value(zinnia_recognizer_t *recognizer, size_t i)
{
    return recognizer->ptr->value(i);
}

const char *zinnia_recognizer_strerror(zinnia_recognizer_t *recognizer)
{
    return recognizer->ptr->what();
}

zinnia_result_t *zinnia_recognizer_classify(zinnia_recognizer_t *recognizer,
                                            const zinnia_character_t *character,
                                            size_t nbest)
{
    zinnia::Result *r = recognizer->ptr->classify(*(character->ptr), nbest);
    if (r) {
        zinnia_result_t *result = new zinnia_result_t;
        result->ptr = r;
        return result;
    } else {
        return 0;
    }
}

/* Trainer */
zinnia_trainer_t *zinnia_trainer_new()
{
    zinnia_trainer_t *trainer = new zinnia_trainer_t;
    trainer->ptr = zinnia::Trainer::create();
    return trainer;
}

void zinnia_trainer_destroy(zinnia_trainer_t *trainer)
{
    delete trainer->ptr;
    delete trainer;
    trainer = 0;
}

int zinnia_trainer_add(zinnia_trainer_t *trainer,
                       const zinnia_character_t *character)
{
    return trainer->ptr->add(*(character->ptr));
}

void zinnia_trainer_clear(zinnia_trainer_t *trainer)
{
    return trainer->ptr->clear();
}

int zinnia_trainer_train(zinnia_trainer_t *trainer, const char *filename)
{
    return trainer->ptr->train(filename);
}

const char *zinnia_trainer_strerror(zinnia_trainer_t *trainer)
{
    return trainer->ptr->what();
}

int zinnia_trainer_convert_model(const char *txt_model,
                                 const char *binary_model,
                                 double compression_threshold)
{
    return zinnia::Trainer::convert(txt_model,
                                    binary_model,
                                    compression_threshold);
}

int zinnia_trainer_make_header(const char *txt_model,
                               const char *header_file,
                               const char *name,
                               double compression_threshold)
{
    return zinnia::Trainer::makeHeader(txt_model,
                                       header_file,
                                       name,
                                       compression_threshold);
}

const char *zinnia_version()
{
    return VERSION;
}

namespace zinnia {
const char *version()
{
    return VERSION;
}
} // namespace zinnia

int zinnia_do(int argc, char **argv)
{
    using namespace zinnia;

    static const zinnia::Option long_options[]
        = {{"output", 'o', "", "FILE", "set FILE as output file"},
           {"model", 'm', 0, "FILE", "use FILE as model file"},
           {"nbest", 'n', "10", "NUM", "obtain best n results"},
           {"version", 'v', 0, 0, "show the version and exit"},
           {"help", 'h', 0, 0, "show this help and exit."},
           {0, 0, 0, 0}};

    zinnia::Param param;

    if (!param.open(argc, argv, long_options)) {
        std::cout << param.what() << "\n\n"
                  << COPYRIGHT << "\ntry '--help' for more information."
                  << std::endl;
        return -1;
    }

    if (!param.help_version())
        return 0;

    std::string model = param.get<std::string>("model");
    size_t nbest = param.get<size_t>("nbest");

    std::string output = param.get<std::string>("output");
    if (output.empty())
        output = "-";
    zinnia::ostream_wrapper ofs(output.c_str());
    CHECK_DIE(*ofs) << "no such file or directory: " << output;

    const std::vector<std::string> &_rest = param.rest_args();
    std::vector<std::string> rest = _rest;
    if (rest.empty())
        rest.push_back("-");

    std::string line;
    zinnia::scoped_ptr<zinnia::Character> character(zinnia::Character::create());
    zinnia::scoped_ptr<zinnia::Recognizer> recognizer(
        zinnia::Recognizer::create());

    CHECK_DIE(recognizer->open(model.c_str())) << recognizer->what();

    for (size_t i = 0; i < rest.size(); ++i) {
        zinnia::istream_wrapper ifs(rest[i].c_str());
        CHECK_DIE(*ifs) << "no such file or directory: " << rest[i];
        while (std::getline(*ifs, line)) {
            CHECK_DIE(character->parse(line.c_str(), line.size()))
                << character->what();
            *ofs << "Answer: " << character->value() << std::endl;
            zinnia::scoped_ptr<zinnia::Result> result(
                recognizer->classify(*(character.get()), nbest));
            if (!result.get()) {
                std::cerr << "failed: " << character->value() << " "
                          << recognizer->what();
                continue;
            }
            for (size_t i = 0; i < result->size(); ++i)
                *ofs << result->value(i) << " " << result->score(i)
                     << std::endl;
        }
    }

    return 0;
}

int zinnia_learn(int argc, char **argv)
{
    using namespace zinnia;

    static const zinnia::Option long_options[]
        = {{"version", 'v', 0, 0, "show the version and exit"},
           {"help", 'h', 0, 0, "show this help and exit."},
           {0, 0, 0, 0}};

    zinnia::Param param;

    if (!param.open(argc, argv, long_options)) {
        std::cout << param.what() << "\n\n"
                  << COPYRIGHT << "\ntry '--help' for more information."
                  << std::endl;
        return -1;
    }

    if (!param.help_version())
        return 0;

    const std::vector<std::string> &rest = param.rest_args();
    CHECK_DIE(rest.size() == 2)
        << "Usage: " << argv[0] << " trainig-file model-file";

    const std::string train_file = rest[0];
    const std::string model_file = rest[1];

    std::string line;
    zinnia::scoped_ptr<zinnia::Character> character(zinnia::Character::create());
    zinnia::scoped_ptr<zinnia::Trainer> trainer(zinnia::Trainer::create());

    std::ifstream ifs(train_file.c_str());
    CHECK_DIE(ifs) << "no such file or directory: " << train_file;
    while (std::getline(ifs, line)) {
        if (!character->parse(line.c_str(), line.size())) {
            std::cerr << character->what() << std::endl;
            continue;
        }
        if (!trainer->add(*character)) {
            std::cerr << character->what() << " " << trainer->what()
                      << std::endl;
            continue;
        }
    }

    CHECK_DIE(trainer->train(model_file.c_str())) << trainer->what();

    return 0;
}

int zinnia_convert(int argc, char **argv)
{
    using namespace zinnia;

    static const zinnia::Option long_options[] = {
        {"version", 'v', 0, 0, "show the version and exit"},
        {"make-header", 'H', 0, 0, "make header mode"},
        {"header-name", 'n', "zinnia_model", "STR", "header name"},
        {"compression-threshold", 'c', "0.00001", "FLOAT", "compression level"},
        {"help", 'h', 0, 0, "show this help and exit."},
        {0, 0, 0, 0}};

    zinnia::Param param;

    if (!param.open(argc, argv, long_options)) {
        std::cout << param.what() << "\n\n"
                  << COPYRIGHT << "\ntry '--help' for more information."
                  << std::endl;
        return -1;
    }

    if (!param.help_version())
        return 0;

    const std::vector<std::string> &rest = param.rest_args();
    CHECK_DIE(rest.size() == 2)
        << "Usage: " << argv[0] << " text-model-file binary-model-file";

    const std::string text_file = rest[0];
    const std::string model_file = rest[1];

    const double c = param.get<double>("compression-threshold");

    if (param.get<bool>("make-header")) {
        const std::string &header_name = param.get<std::string>("header-name");
        CHECK_DIE(zinnia::Trainer::makeHeader(text_file.c_str(),
                                              model_file.c_str(),
                                              header_name.c_str(),
                                              c));
        return 0;
    }

    CHECK_DIE(
        zinnia::Trainer::convert(text_file.c_str(), model_file.c_str(), c));

    return 0;
}

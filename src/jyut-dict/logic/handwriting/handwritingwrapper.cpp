#include "handwritingwrapper.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QStandardPaths>

#include <iostream>
#include <sstream>

namespace {
constexpr auto SIMPLIFIED_MODEL = "handwriting-zh_CN.model";
constexpr auto TRADITIONAL_MODEL = "handwriting-zh_TW.model";
constexpr auto CHARACTER_PREAMBLE
    = "(character (width %1)(height %1) (strokes ";
constexpr auto CHARACTER_POSTSCRIPT = "))";
} // namespace

HandwritingWrapper::HandwritingWrapper(Handwriting::Script script)
    : QObject()
    , _recognizer{zinnia::Recognizer::create()}
{
    if (!setRecognizerScript(script)) {
        std::cerr << _recognizer->what() << std::endl;
    }
}

HandwritingWrapper::~HandwritingWrapper()
{
    if (_character) {
        delete _character;
    }
    if (_recognizer) {
        delete _recognizer;
    }
}

bool HandwritingWrapper::setRecognizerScript(Handwriting::Script script)
{
    if (!_recognizer) {
        return false;
    }

    std::string modelFile;
    switch (script) {
    case Handwriting::Script::SIMPLIFIED: {
        modelFile = getModelPath().toStdString() + SIMPLIFIED_MODEL;
        break;
    }
    case Handwriting::Script::TRADITIONAL: {
        modelFile = getModelPath().toStdString() + TRADITIONAL_MODEL;
        break;
    }
    }

    _recognizer->close();
    int status = _recognizer->open(modelFile.c_str());
    classifyCharacter();
    return status;
}

void HandwritingWrapper::clearStrokes(void)
{
    _strokes.clear();
    _currentStrokePoints.clear();
}

bool HandwritingWrapper::strokesCleared(void)
{
    return _strokes.empty() && _currentStrokePoints.empty();
}

QString HandwritingWrapper::getModelPath() const
{
#ifdef PORTABLE
    return getBundleModelPath();
#else
    return getLocalModelPath();
#endif
}

QString HandwritingWrapper::getLocalModelPath(void) const
{
#ifdef Q_OS_DARWIN
    QFileInfo localFile{
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
        + "/Handwriting/"};
#elif defined(Q_OS_WIN)
    QFileInfo localFile{
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
        + "/Handwriting/"};
#else
    QFileInfo localFile{
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
        + "/Handwriting/"};
#endif
    return localFile.absoluteFilePath();
}

QString HandwritingWrapper::getBundleModelPath(void) const
{
#ifdef Q_OS_DARWIN
    QFileInfo bundlePath{QCoreApplication::applicationDirPath()
                         + "/../Resources/Handwriting/"};
#elif defined(Q_OS_WIN)
    QFileInfo bundlePath{QCoreApplication::applicationDirPath()
                         + "./Handwriting/"};
#else // Q_OS_LINUX
#ifdef APPIMAGE
    QFileInfo bundlePath{QCoreApplication::applicationDirPath()
                         + "/../share/jyut-dict/handwriting/"};
#elif defined(DEBUG)
    QFileInfo bundlePath{"./handwriting/"};
#elif defined(FLATPAK)
    QFileInfo bundlePath{QCoreApplication::applicationDirPath()
                         + "/../share/jyut-dict/handwriting/"};
#else
    QFileInfo bundlePath{"/usr/share/jyut-dict/handwriting/"};
#endif
#endif
    return bundlePath.absoluteFilePath();
}

void HandwritingWrapper::classifyCharacter(void)
{
    if (_strokes.empty()) {
        return;
    }

    std::ostringstream character_ss;
    character_ss << QString{CHARACTER_PREAMBLE}.arg(_width).toStdString();
    for (const auto &stroke : _strokes) {
        character_ss << stroke;
    }
    character_ss << CHARACTER_POSTSCRIPT;

    zinnia::Character *character = zinnia::Character::create();
    if (!character->parse(character_ss.str().c_str())) {
        std::cerr << "Could not parse character: " << character->what()
                  << std::endl;
        ;
    }

    zinnia::Result *res = _recognizer->classify(*character, 10);
    if (!res) {
        std::cerr << "Could not classify character: " << _recognizer->what()
                  << std::endl;
    }
    std::vector<std::string> results;
    for (size_t i = 0; i < res->size(); ++i) {
        results.emplace_back(res->value(i));
    }
    delete character;

    emit recognizedResults(results);
}

void HandwritingWrapper::setDimensions(int width, int height)
{
    _width = width;
    _height = height;
}

void HandwritingWrapper::startStroke(int x, int y)
{
    _currentStrokePoints.clear();
    _currentStrokePoints.emplace_back(x, y);
}

void HandwritingWrapper::updateStroke(int x, int y)
{
    if (_currentStrokePoints.back().first != x
        || _currentStrokePoints.back().second != y) {
        _currentStrokePoints.emplace_back(x, y);
    }
}

void HandwritingWrapper::completeStroke(int x, int y)
{
    // Zinnia gets very angry if (a) a stroke ends the same place it began,
    // or (b) when there is only one point in a stroke
    if (_currentStrokePoints.back().first != x
        || _currentStrokePoints.back().second != y) {
        _currentStrokePoints.emplace_back(x, y);
    }

    if (_currentStrokePoints.size() <= 1) {
        return;
    }

    std::ostringstream stroke;
    stroke << "(";
    for (const auto &[x, y] : _currentStrokePoints) {
        stroke << "(" << std::to_string(x) << " " << std::to_string(y) << ")";
    }
    stroke << ")";
    _strokes.emplace_back(stroke.str());
    _currentStrokePoints.clear();

    classifyCharacter();
}

#include "handwritingwrapper.h"

#ifdef Q_OS_WIN
#include "logic/strings/strings.h"
#endif

#include <QCoreApplication>
#include <QFileInfo>
#include <QStandardPaths>
#include <QtConcurrent/QtConcurrent>

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
    setRecognizerScript(script);

    showProgressDialog(tr("Preparing handwriting models..."));
    _boolReturnWatcher = new QFutureWatcher<Utils::Result<bool>>{this};
    disconnect(_boolReturnWatcher, nullptr, nullptr, nullptr);
    connect(_boolReturnWatcher,
            &QFutureWatcher<Utils::Result<bool>>::finished,
            this,
            [=, this]() {
                _progressDialog->reset();
                Utils::Result<bool> result = _boolReturnWatcher->result();
                if (std::system_error *e = std::get_if<std::system_error>(
                        &result)) {
                    emit modelError(*e);
                }
            });
    QFuture<Utils::Result<bool>> future = QtConcurrent::run(
        [=, this]() { return copyModels(); });
    _boolReturnWatcher->setFuture(future);
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
    if (!status) {
        std::system_error e{ENOENT,
                            std::generic_category(),
                            _recognizer->what()};
        emit modelError(e);
        return false;
    }

    classifyCharacter();
    return true;
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

void HandwritingWrapper::showProgressDialog(QString text)
{
    _progressDialog = new QProgressDialog{"", QString(), 0, 0};
    _progressDialog->setWindowModality(Qt::ApplicationModal);
    _progressDialog->setMinimumSize(300, 75);
    Qt::WindowFlags flags = _progressDialog->windowFlags()
                            | Qt::CustomizeWindowHint;
    flags &= ~(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint
               | Qt::WindowFullscreenButtonHint
               | Qt::WindowContextHelpButtonHint);
    _progressDialog->setWindowFlags(flags);
    _progressDialog->setMinimumDuration(500);
#ifdef Q_OS_WIN
    _progressDialog->setWindowTitle(
        QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                    Strings::PRODUCT_NAME));
#elif defined(Q_OS_LINUX)
    _progressDialog->setWindowTitle(" ");
#endif
    _progressDialog->setAttribute(Qt::WA_DeleteOnClose, true);
    _progressDialog->setLabelText(text);
    _progressDialog->setRange(0, 0);
    _progressDialog->setValue(0);
}

Utils::Result<bool> HandwritingWrapper::copyModels(void) const
{
#ifdef PORTABLE
    QFileInfo traditionalFile{getModelPath() + TRADITIONAL_MODEL};
    QFileInfo simplifiedFile{getModelPath() + SIMPLIFIED_MODEL};
    if (!traditionalFile.exists() || !traditionalFile.isFile()) {
        return std::system_error{
            ENOENT,
            std::generic_category(),
            "Traditional handwriting model could not be found"};
    }
    if (!simplifiedFile.exists() || !simplifiedFile.isFile()) {
        return std::system_error{
            ENOENT,
            std::generic_category(),
            "Simplified handwriting model could not be found"};
    }
    return true;
#else
    QFileInfo localDir{getLocalModelPath()};
    QFileInfo bundleDir{getBundleModelPath()};

    // Make path for model storage
    if (!localDir.exists()) {
        if (!QDir().mkpath(localDir.absolutePath())) {
            return std::system_error{
                ENOTDIR,
                std::generic_category(),
                "Could not create directory for handwriting model storage"};
        }
    }

    QFileInfo traditionalLocalFile{getLocalModelPath() + TRADITIONAL_MODEL};
    QFileInfo traditionalBundleFile{getBundleModelPath() + TRADITIONAL_MODEL};
    QFileInfo simplifiedLocalFile{getLocalModelPath() + SIMPLIFIED_MODEL};
    QFileInfo simplifiedBundleFile{getBundleModelPath() + SIMPLIFIED_MODEL};
    // Copy file from bundle to Application Support
    if (!traditionalLocalFile.exists() || !traditionalLocalFile.isFile()) {
        if (!QFile::copy(traditionalBundleFile.absoluteFilePath(),
                         traditionalLocalFile.absoluteFilePath())) {
            return std::system_error{
                ENOENT,
                std::generic_category(),
                "Failed to copy traditional handwriting model to "
                "Application Support location"};
        }
    }
    if (!simplifiedLocalFile.exists() || !simplifiedLocalFile.isFile()) {
        if (!QFile::copy(simplifiedBundleFile.absoluteFilePath(),
                         simplifiedLocalFile.absoluteFilePath())) {
            return std::system_error{
                ENOENT,
                std::generic_category(),
                "Failed to copy simplified handwriting model to "
                "Application Support location"};
        }
    }

    // Delete file in bundle
    if (traditionalBundleFile.exists() && traditionalBundleFile.isFile()) {
        if (!QFile::remove(traditionalBundleFile.absoluteFilePath())) {
            std::cout << "Could not delete bundled traditional handwriting "
                         "model, non-fatal"
                      << std::endl;
        }
    }
    if (simplifiedBundleFile.exists() && simplifiedBundleFile.isFile()) {
        if (!QFile::remove(simplifiedBundleFile.absoluteFilePath())) {
            std::cout << "Could not delete bundled simplified handwriting "
                         "model, non-fatal"
                      << std::endl;
        }
    }

    return true;
#endif
}

void HandwritingWrapper::classifyCharacter(void) const
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

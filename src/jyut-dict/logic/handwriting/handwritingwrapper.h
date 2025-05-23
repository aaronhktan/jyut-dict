#ifndef HANDWRITINGWRAPPER_H
#define HANDWRITINGWRAPPER_H

#include "vendor/zinnia/zinnia.h"

#include <QObject>
#include <QString>

// The handwriting wrapper receives stroke data, and then
// submits them to the backend library (currently Zinnia)
// to recognize characters.

namespace Handwriting {

enum class Backend : int {
    ZINNIA,
    NUM_BACKENDS,
};

enum class Script : int { SIMPLIFIED, TRADITIONAL };

} // namespace Handwriting

Q_DECLARE_METATYPE(Handwriting::Script)

class HandwritingWrapper : public QObject
{
    Q_OBJECT

public:
    explicit HandwritingWrapper(Handwriting::Script script);
    ~HandwritingWrapper();

    bool setRecognizerScript(Handwriting::Script script);

    void clearStrokes(void);
    bool strokesCleared(void);

    QString getModelPath(void) const;
    QString getLocalModelPath(void) const;
    QString getBundleModelPath(void) const;

private:
    void classifyCharacter(void);

    zinnia::Recognizer *_recognizer = nullptr;
    zinnia::Character *_character = nullptr;

    std::vector<std::pair<int, int>> _currentStrokePoints;
    std::vector<std::string> _strokes;
    int _width;
    int _height;

signals:
    void recognizedResults(std::vector<std::string> &results);

public slots:
    void setDimensions(int width, int height);

    void startStroke(int x, int y);
    void updateStroke(int x, int y);
    void completeStroke(int x, int y);
};

#endif // HANDWRITINGWRAPPER_H

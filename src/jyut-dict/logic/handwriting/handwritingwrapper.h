#ifndef HANDWRITINGWRAPPER_H
#define HANDWRITINGWRAPPER_H

#include "logic/utils/utils.h"

#include "vendor/zinnia/zinnia.h"

#include <QFutureWatcher>
#include <QObject>
#include <QProgressDialog>
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
    Utils::Result<bool> copyModels(void) const;
    void showProgressDialog(QString text);
    void classifyCharacter(void) const;

    zinnia::Recognizer *_recognizer = nullptr;
    zinnia::Character *_character = nullptr;

    QFutureWatcher<Utils::Result<bool>> *_boolReturnWatcher = nullptr;
    QProgressDialog *_progressDialog = nullptr;

    std::vector<std::pair<int, int>> _currentStrokePoints;
    std::vector<std::string> _strokes;
    int _width;
    int _height;

signals:
    void recognizedResults(std::vector<std::string> &results) const;
    void modelError(std::system_error &e) const;

public slots:
    void setDimensions(int width, int height);

    void startStroke(int x, int y);
    void updateStroke(int x, int y);
    void completeStroke(int x, int y);
};

#endif // HANDWRITINGWRAPPER_H

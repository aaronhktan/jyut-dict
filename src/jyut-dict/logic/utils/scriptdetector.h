#ifndef SCRIPTDETECTOR_H
#define SCRIPTDETECTOR_H

#include <QString>

class ScriptDetector
{
public:
    ScriptDetector(const QString &string);

    bool containsSimplifiedChinese();
    bool containsTraditionalChinese();
    bool containsChinese();

    bool isValidJyutping();
    bool isValidJyutpingAfterAutocorrect();
    bool isValidPinyin();

private:
    bool _containsSimplifiedChinese = false;
    bool _containsTraditionalChinese = false;
    bool _containsChinese = false;

    bool _isValidJyutping = false;
    bool _isValidJyutpingAfterAutocorrect = false;
    bool _isValidPinyin = false;
};

#endif // SCRIPTDETECTOR_H

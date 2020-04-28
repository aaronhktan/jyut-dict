#ifndef ENTRYSPEAKER_H
#define ENTRYSPEAKER_H

#include <QString>
#include <QTextToSpeech>

class EntrySpeaker
{
public:
    EntrySpeaker();
    ~EntrySpeaker();

    int speak(const QLocale::Language &language,
              const QLocale::Country &country,
              const QString &string);

    int speakCantonese(const QString &string);
    int speakTaiwaneseMandarin(const QString &string);
    int speakMainlandMandarin(const QString &string);

private:
    QTextToSpeech *_tts;
};

#endif // ENTRYSPEAKER_H

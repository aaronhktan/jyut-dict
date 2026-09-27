#ifndef EXAMPLECONTENTWIDGET_H
#define EXAMPLECONTENTWIDGET_H

#include "logic/entry/entrycharactersoptions.h"
#include "logic/entry/entryphoneticoptions.h"

#include <QSettings>
#include <QWidget>

#include <span>

class Example;
class TranslationSet;

class QEvent;
class QGridLayout;
class QLabel;
class QResizeEvent;

// The ExampleContentWidget displays Examples.
// When used with setTranslationSet, it will display a list of
// Translations, all from that set.
// When used with setExampleVector, it will display each Example,
// and the first Translation from the first TranslationSet belonging to that
// Example.

class ExampleContentWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ExampleContentWidget(QWidget *parent = nullptr);
    ~ExampleContentWidget() override;

    void changeEvent(QEvent *event) override;

    void setTranslationSet(const TranslationSet &set);
    void setExampleVector(std::span<const Example> examples);

private:
    void translateUI(void);
    void setStyle(bool use_dark);

    void addLabelsToLayout(QGridLayout *layout,
                           int rowNumber,
                           QLabel *exampleNumberLabel,
                           QLabel *simplifiedLabel,
                           QLabel *traditionalLabel,
                           QLabel *cantoneseLabel,
                           QLabel *mandarinLabel,
                           QLabel *exampleLabel,
                           QLabel *exampleLanguage,
                           EntryPhoneticOptions phoneticOptions,
                           EntryCharactersOptions characterOptions);

    void cleanupLabels();
    void clearLabelVector(std::vector<QLabel *> &vector);

    bool _paletteRecentlyChanged = false;

    std::unique_ptr<QSettings> _settings;

    QGridLayout *_exampleLayout;
    std::vector<QLabel *> _exampleNumberLabels;
    std::vector<QLabel *> _simplifiedLabels;
    std::vector<QLabel *> _traditionalLabels;
    std::vector<QLabel *> _cantoneseLabels;
    std::vector<QLabel *> _mandarinLabels;
    std::vector<QLabel *> _exampleLabels;
    std::vector<QLabel *> _exampleLanguage;
    std::vector<QLabel *> _spaceLabels;
};

#endif // EXAMPLECONTENTWIDGET_H

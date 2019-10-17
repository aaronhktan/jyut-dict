#include "definitionscrollarea.h"

#include "components/definitionscrollareawidget.h"
#include "logic/entry/definitionsset.h"
#include "logic/entry/entry.h"
#include "logic/entry/sentence.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#endif

#include <QScrollBar>
#include <QTimer>

#define ENTIRE_WIDTH -1

DefinitionScrollArea::DefinitionScrollArea(QWidget *parent) : QScrollArea(parent)
{
    setFrameShape(QFrame::NoFrame);

    _scrollAreaWidget = new DefinitionScrollAreaWidget{this};

    setWidget(_scrollAreaWidget);
    setWidgetResizable(true); // IMPORTANT! This makes the scrolling widget resize correctly.
    setStyleSheet("QScrollArea { background-color: #1E1E1E; }");
#ifdef Q_OS_LINUX
    setMinimumWidth(250);
#else
    setMinimumWidth(350);
#endif

#ifdef Q_OS_MAC
    setStyle(Utils::isDarkMode());
#else
    setStyle(/* use_dark = */false);
#endif

//    testEntry();
}

DefinitionScrollArea::~DefinitionScrollArea()
{
}

void DefinitionScrollArea::changeEvent(QEvent *event)
{
#if defined(Q_OS_DARWIN)
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(100, [=]() { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
#endif
    QScrollArea::changeEvent(event);
}

void DefinitionScrollArea::testEntry() {
    // Create dummy entry
    std::string simplified{"风水佬呃你十年八年，唔呃得一世"};
    std::string traditional{"風水佬呃你十年八年，唔呃得一世"};
    std::string jyutping{"fung1 seoi2 lou2 ak1 nei5 sap6 nin4 baat3 nin4\u2060，m4 ak1 dak1 jat1 sai3"};
    std::string pinyin{"feng1 shui5 lao3 e4 ni3 shi2 nian2 ba1 nian2\u2060, m2 e4 de5 yi1 shi4"};
    std::vector<std::string>definitions_CC{"literally: 'a Fung Shui master can fool you for only eight to ten years, but not for a whole life'; time will tell; you will realise with time that I am telling you the truth; time will witness the truth; would I lie to you?",
                                          "an expression to describe a comfortable (place); an expression to describe a cool, windy, pleasant place (can also be used in an ironic sense)",
                                          "view; chaos of war; smoke that drifts away with the wind(\"烽烟\"beacon-fire)",
                                          "(noun) a hair dryer; a blowdryer",
                                          "a prefect 8",
                                          "Chinese preserved sausage (slang)",
                                          "a windbreaker (clothing)",
                                          "slanderous gossip",
                                          "hives",
                                          "aeolian bells; wind chimes",
                                          "(eat and sleep in the open air); endure the hardships of traveling or working outdoors",
                                          "literally: 'floating colour', a type of traditional folk art which is a fusion of drama,conjuring,acrobatics,music and dance",
                                          "(verb) (of relationship) 1. to break up (with a boy /girl friend); or being dumped (by a boy/girl friend); (of hair) 1. cut; (slang)  ticket; (slang) to kick someone off from (a show, etc.)",
                                          "(noun) a male juvenile rogue; a young gangster; a young thug boy; a juvenile delinquent"};
    std::vector<std::string>definitions_CCCANTO{"Hi!",
                                                "THIS IS A TEST",
                                                "WOOOOOOO"};
    std::vector<DefinitionsSet>definitions{DefinitionsSet{"CEDICT", definitions_CC},
                                           DefinitionsSet{"CC-CANTO", definitions_CCCANTO}};
    std::vector<DefinitionsSet>definitions2{DefinitionsSet{"CEDICT", definitions_CC}};
    std::vector<std::string>derivedWords{};
    std::vector<Sentence>sentences{};

    Entry entry(simplified, traditional, jyutping, pinyin, definitions, derivedWords, sentences);
    Entry entry2(simplified, traditional, jyutping, pinyin, definitions2, derivedWords, sentences);
    setEntry(entry2);

    setEntry(entry);
}

void DefinitionScrollArea::setEntry(const Entry &entry)
{
    _scrollAreaWidget->setEntry(entry);
    _scrollAreaWidget->resize(width()
                                  - (verticalScrollBar()->isVisible() ? verticalScrollBar()->width() : 0),
                              _scrollAreaWidget->sizeHint().height());
}

void DefinitionScrollArea::resizeEvent(QResizeEvent *event)
{
    _scrollAreaWidget->resize(width()
                              - (verticalScrollBar()->isVisible() ? verticalScrollBar()->width() : 0),
                              _scrollAreaWidget->sizeHint().height());
    event->accept();
}

void DefinitionScrollArea::setStyle(bool use_dark)
{
    if (use_dark) {
        setStyleSheet("QWidget { background-color: #1E1E1E; }");
    } else {
        setStyleSheet("QWidget { background-color: #FFFFFF; }");
    }
}

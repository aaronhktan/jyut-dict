#include "definitionscrollarea.h"

#include "logic/entry/definitionsset.h"
#include "logic/entry/entry.h"
#include "logic/entry/sentence.h"

#include <QScrollBar>

#define ENTIRE_WIDTH -1

DefinitionScrollArea::DefinitionScrollArea(QWidget *parent) : QScrollArea(parent)
{
    setFrameShape(QFrame::NoFrame);

    // Entire Scroll Area
    _scrollAreaLayout = new QVBoxLayout{this};
    _scrollAreaLayout->setSpacing(25);
    _scrollAreaLayout->setContentsMargins(11, 11, 11, 11);

    _scrollAreaWidget = new QWidget{this};
    _scrollAreaWidget->setLayout(_scrollAreaLayout);
//    _scrollAreaWidget->resize(width(), _scrollAreaWidget->sizeHint().height());
    _scrollAreaWidget->setStyleSheet("QWidget { background-color: red; }");
    setStyleSheet("QScrollArea { background-color: blue; }");

    setWidget(_scrollAreaWidget);
#ifdef Q_OS_LINUX
    setMinimumWidth(250);
#else
    setMinimumWidth(350);
#endif

    _entryHeaderWidget = new EntryHeaderWidget{this};
    _definitionWidget = new DefinitionWidget{this};

    // Add all widgets to main layout
    _scrollAreaLayout->addWidget(_entryHeaderWidget);
    _scrollAreaLayout->addWidget(_definitionWidget);
    _scrollAreaLayout->addStretch(1);

//    testEntry();
}

DefinitionScrollArea::~DefinitionScrollArea()
{

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

#include <QDebug>
#include <QCoreApplication>
#include <QApplication>
void DefinitionScrollArea::setEntry(const Entry &entry)
{
    _entryHeaderWidget->setEntry(entry);
    _definitionWidget->setEntry(entry);
    //    qDebug() << _entryHeaderWidget->sizeHint().height();
    //    qDebug() << _definitionWidget->sizeHint().height();
    //    _scrollAreaWidget->resize(QSize{width(),
    //                                          _entryHeaderWidget->sizeHint().height() +
    //                                              _definitionWidget->sizeHint().height()});
    this->resize(this->geometry().width(), this->geometry().height());
    _newSize = new QSize{this->geometry().width(), this->geometry().height()};
    _oldSize = new QSize{this->geometry().width(), this->geometry().height()};
    QResizeEvent *myResizeEvent = new QResizeEvent{*_newSize, *_oldSize};
    QApplication::postEvent(this, myResizeEvent);
    _scrollAreaWidget->resize(width()
                                  - (verticalScrollBar()->isVisible() ? verticalScrollBar()->width() : 0),
                              _scrollAreaWidget->sizeHint().height());
    this->updateGeometry();
//    _scrollAreaWidget->resize(width(), _scrollAreaWidget->heightForWidth(width()));
}

void DefinitionScrollArea::resizeEvent(QResizeEvent *event) {
    //    _scrollAreaWidget->setFixedSize(QSize{width(),
    //                                          _entryHeaderWidget->sizeHint().height() +
    //                                              _definitionWidget->sizeHint().height()});
    qDebug() << "Resized!";
    _scrollAreaWidget->resize(width()
                              - (verticalScrollBar()->isVisible() ? verticalScrollBar()->width() : 0),
                              _scrollAreaWidget->sizeHint().height());
    QScrollArea::resizeEvent(event);
    event->accept();
}

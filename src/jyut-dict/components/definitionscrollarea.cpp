#include "definitionscrollarea.h"

#include "logic/entry/entry.h"
#include "logic/entry/sentence.h"

#include <iostream>

#include <QFontMetrics>
#include <QSpacerItem>
#include <QTabWidget>

#define ENTIRE_WIDTH -1

DefinitionScrollArea::DefinitionScrollArea(QWidget *parent) : QScrollArea(parent)
{
    setFrameShape(QFrame::NoFrame);

    // Entire Scroll Area
    scrollAreaLayout = new QVBoxLayout(this);
    scrollAreaLayout->setSpacing(25);

    scrollAreaWidget = new QWidget(this);
    scrollAreaWidget->setLayout(scrollAreaLayout);
    scrollAreaWidget->resize(this->width(), scrollAreaWidget->sizeHint().height());

    setWidget(scrollAreaWidget);
    setMinimumWidth(350);

    // Entry header section
    entryHeaderLayout = new QGridLayout(this);
    entryHeaderLayout->setContentsMargins(0, 0, 0, 0);

    wordLabel = new QLabel(this);
    wordLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    wordLabel->setWordWrap(true);
    wordLabel->setStyleSheet("QLabel { font-size: 24px}");

    jyutpingLabel = new QLabel("<font color=#6f6f6f>JP</font>", this);
    jyutpingLabel->setFixedWidth(jyutpingLabel->fontMetrics().boundingRect("JP").width());
    jyutpingLabel->setFixedHeight(jyutpingLabel->fontMetrics().boundingRect("ly,").height());
    jyutpingPronunciation = new QLabel(this);
    jyutpingPronunciation->setWordWrap(true);

    pinyinLabel = new QLabel("<font color=#6f6f6f>PY</font>");
    pinyinLabel->setFixedWidth(pinyinLabel->fontMetrics().boundingRect("PY").width());
    pinyinPronunciation = new QLabel(this);
    pinyinPronunciation->setWordWrap(true);

    entryHeaderLayout->addWidget(wordLabel, 1, 0, 1, -1);
    entryHeaderLayout->addWidget(jyutpingLabel, 2, 0, 1, 1, Qt::AlignTop);
    entryHeaderLayout->addWidget(jyutpingPronunciation, 2, 1, 1, 1);
    entryHeaderLayout->addWidget(pinyinLabel, 3, 0, 1, 1, Qt::AlignTop);
    entryHeaderLayout->addWidget(pinyinPronunciation, 3, 1, 1, 1);

    entryHeader = new QWidget(this);
    entryHeader->setLayout(entryHeaderLayout);

    // Definition Header
    definitionHeader = new QWidget(this);

    definitionHeaderLayout = new QVBoxLayout(this);
    definitionHeaderLayout->setContentsMargins(0, 0, 0, 0);
    definitionHeaderLayout->setSpacing(5);
    definitionHeader->setLayout(definitionHeaderLayout);

    definitionHeaderLabel = new QLabel("<font color=#6f6f6f>DEFINITION</font>", this);
    definitionHeaderLabel->setFixedHeight(definitionHeaderLabel->fontMetrics().boundingRect("DEFINITION").height());

    separatorLine = new QFrame(this);
    separatorLine->setFrameShape(QFrame::HLine);
    separatorLine->setFrameShadow(QFrame::Raised);

    definitionHeaderLayout->addWidget(definitionHeaderLabel);
    definitionHeaderLayout->addWidget(separatorLine);

    // Definitions
    definition = new QWidget(this);

    definitionLayout = new QGridLayout(this);
    definitionLayout->setContentsMargins(0, 0, 0, 0);
    definition->setLayout(definitionLayout);

    definitionNumberLabels = {};
    definitionLabels = {};

    // Entire definition section
    definitionArea = new QWidget(this);

    definitionAreaLayout = new QVBoxLayout(this);
    definitionAreaLayout->setContentsMargins(0, 0, 0, 0);
    definitionAreaLayout->addWidget(definitionHeader);
    definitionAreaLayout->addWidget(definition);
    definitionArea->setLayout(definitionAreaLayout);

//     Extra stuff for testing
    QTabWidget *widget = new QTabWidget(this);
    widget->setMinimumHeight(50);

    // Add all widgets to main layout
    scrollAreaLayout->addWidget(entryHeader);
    scrollAreaLayout->addWidget(definitionArea);
    scrollAreaLayout->addWidget(widget);

    // Create dummy entry
    std::string word{"風水佬呃你十年八年，唔呃得一世"};
    std::string jyutping{"fung1 seoi2 lou2 ak1 nei5 sap6 nin4 baat3 nin4\u2060，m4 ak1 dak1 jat1 sai3"};
    std::string pinyin{"feng1 shui5 lao3 e4 ni3 shi2 nian2 ba1 nian2\u2060, m2 e4 de5 yi1 shi4"};
    std::vector<std::string>definitions{"literally: 'a Fung Shui master can fool you for only eight to ten years, but not for a whole life'; time will tell; you will realise with time that I am telling you the truth; time will witness the truth; would I lie to you?",
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
    std::vector<std::string>derivedWords{};
    std::vector<Sentence>sentences{};

    Entry entry(word, jyutping, pinyin, definitions, derivedWords, sentences);
    setEntry(entry);
}

void DefinitionScrollArea::setEntry(Entry& entry)
{
    // Set texts
    wordLabel->setText(entry.getWord().c_str());
    jyutpingPronunciation->setText(entry.getJyutping().c_str());
    pinyinPronunciation->setText(entry.getPinyin().c_str());
    auto definitions = entry.getDefinitions();

    for (size_t i = 0; i < definitions.size(); i++) {
        std::string number{"<font color=#6f6f6f>" + std::to_string(i + 1) + "</font>"};
        definitionNumberLabels.push_back(new QLabel(number.c_str(), this));
        int definitionNumberWidth = definitionNumberLabels.back()->fontMetrics().boundingRect("PY").width();
        definitionNumberLabels.back()->setFixedWidth(definitionNumberWidth);
        definitionLabels.push_back(new QLabel(definitions[i].c_str(), this));
        definitionLabels.back()->setWordWrap(true);
        definitionLabels.back()->setTextInteractionFlags(Qt::TextSelectableByMouse);
        definitionLayout->addWidget(definitionNumberLabels[i], static_cast<int>(i + 9), 0, Qt::AlignTop);
        definitionLayout->addWidget(definitionLabels[i], static_cast<int>(i + 9), 1, Qt::AlignTop);
    }
}

void DefinitionScrollArea::resizeEvent(QResizeEvent *event) {
    scrollAreaWidget->resize(this->width(), scrollAreaWidget->sizeHint().height());
    event->accept();
}

DefinitionScrollArea::~DefinitionScrollArea()
{
    delete scrollAreaWidget;
}

#include "definitionscrollarea.h"

#include <iostream>

#include <QFontMetrics>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QTabWidget>
#include <QVBoxLayout>

#define ENTIRE_WIDTH -1

DefinitionScrollArea::DefinitionScrollArea(QWidget *parent) : QScrollArea(parent)
{
    setFrameShape(QFrame::NoFrame);

    // Entry header section
    std::string jyutping{"fung1 seoi2 lou2 ak1 nei5 sap6 nin4 baat3 nin4\u2060，m4 ak1 dak1 jat1 sai3"};
    std::string pinyin{"feng1 shui5 lao3 e4 ni3 shi2 nian2 ba1 nian2\u2060, m2 e4 de5 yi1 shi4"};

    QLabel *wordLabel = new QLabel("風水佬呃你十年八年，唔呃得一世");
    wordLabel->setStyleSheet("QLabel { \
                                font-size: 24px; \
                             }");
    wordLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    wordLabel->setWordWrap(true);

    QLabel *jyutpingLabel = new QLabel("<font color=#6f6f6f>JP</font>");
    jyutpingLabel->setWordWrap(true);
    int jpWidth = jyutpingLabel->fontMetrics().boundingRect("JP").width();
    jyutpingLabel->setFixedWidth(jpWidth);
    QLabel *jyutpingPronunciation = new QLabel(jyutping.c_str());
    jyutpingPronunciation->setWordWrap(true);

    QLabel *pinyinLabel = new QLabel("<font color=#6f6f6f>PY</font>");
    pinyinLabel->setWordWrap(true);
    int pyWidth = pinyinLabel->fontMetrics().boundingRect("PY").width();
    pinyinLabel->setFixedWidth(pyWidth);
    QLabel *pinyinPronunciation = new QLabel(pinyin.c_str());
    pinyinPronunciation->setWordWrap(true);

    QGridLayout *entryHeaderLayout = new QGridLayout;
    entryHeaderLayout->setContentsMargins(0, 0, 0, 0);

    entryHeaderLayout->addWidget(wordLabel, 1, 0, 1, ENTIRE_WIDTH, Qt::AlignTop);
    entryHeaderLayout->addWidget(jyutpingLabel, 2, 0, 1, 1, Qt::AlignTop);
    entryHeaderLayout->addWidget(jyutpingPronunciation, 2, 1, 1, 1, Qt::AlignTop);
    entryHeaderLayout->addWidget(pinyinLabel, 3, 0, 1, 1, Qt::AlignTop);
    entryHeaderLayout->addWidget(pinyinPronunciation, 3, 1, 1, 1, Qt::AlignTop);

    QWidget *entryHeader = new QWidget;
    entryHeader->setLayout(entryHeaderLayout);

    // Definition Header
    QLabel *definitionHeaderLabel = new QLabel("<font color=#6f6f6f>DEFINITION</font>");

    QFrame *separatorLine = new QFrame;
    separatorLine->setFrameShape(QFrame::HLine);
    separatorLine->setFrameShadow(QFrame::Raised);

    QVBoxLayout *definitionHeaderLayout = new QVBoxLayout;
    definitionHeaderLayout->setContentsMargins(0, 0, 0, 0);
    definitionHeaderLayout->setSpacing(5);
    definitionHeaderLayout->addWidget(definitionHeaderLabel);
    definitionHeaderLayout->addWidget(separatorLine);

    QWidget *definitionHeader = new QWidget;
    definitionHeader->setLayout(definitionHeaderLayout);

    // Definition
    QGridLayout *definitionLayout = new QGridLayout;
    definitionLayout->setContentsMargins(0, 0, 0, 0);
    std::vector<std::string>definitions{"literally: 'a Fung Shui master can fool you for only eight to ten years, but not for a whole life'; time will tell; you will realise with time that I am telling you the truth; time will witness the truth; would I lie to you?",
                                        "an expression to describe a comfortable (place); an expression to describe a cool, windy, pleasant place (can also be used in an ironic sense)",
                                        "view; chaos of war; smoke that drifts away with the wind(\"烽烟\"beacon-fire)",
                                        "(noun) a hair dryer; a blowdryer",
                                        "a prefect 8",
                                        "Chinese preserved sausage (slang)",
                                        "a windbreaker (clothing)",
                                        "slanderous gossip",
                                        "hives",
                                        "aeolian bells; wind chimes"};
    std::vector<QLabel *>definitionNumberLabels{};
    std::vector<QLabel *>definitionLabels{};
    for (size_t i = 0; i < definitions.size(); i++) {
        std::string number{"<font color=#6f6f6f>" + std::to_string(i + 1) + "</font>"};
        definitionNumberLabels.push_back(new QLabel(number.c_str()));
        int definitionNumberWidth = definitionNumberLabels.back()->fontMetrics().boundingRect("PY" /*QString::number(i + 1)*/).width();
        definitionNumberLabels.back()->setFixedWidth(definitionNumberWidth);
        definitionLabels.push_back(new QLabel(definitions[i].c_str()));
        definitionLabels.back()->setWordWrap(true);
        definitionLabels.back()->setTextInteractionFlags(Qt::TextSelectableByMouse);
        definitionLayout->addWidget(definitionNumberLabels[i], static_cast<int>(i), 0, Qt::AlignTop);
        definitionLayout->addWidget(definitionLabels[i], static_cast<int>(i), 1, Qt::AlignTop);
    }
    QWidget *definition = new QWidget;
    definition->setLayout(definitionLayout);

    // Definition Section (Header + Definition)
    QVBoxLayout *definitionSectionLayout = new QVBoxLayout;
    definitionSectionLayout->setContentsMargins(0, 0, 0, 0);
    definitionSectionLayout->addWidget(definitionHeader);
    definitionSectionLayout->addWidget(definition);

    QWidget *definitionSection = new QWidget;
    definitionSection->setLayout(definitionSectionLayout);

    // More information tabs
    QTabWidget *informationTabs = new QTabWidget;
    informationTabs->setMinimumHeight(500);

    // Entire scroll area
    QVBoxLayout *scrollAreaLayout = new QVBoxLayout;
    scrollAreaLayout->setSpacing(25);
    scrollAreaLayout->addWidget(entryHeader);
    scrollAreaLayout->addWidget(definitionSection);
    scrollAreaLayout->addWidget(informationTabs);

    scrollAreaWidget = new QWidget();
    scrollAreaWidget->setLayout(scrollAreaLayout);
    scrollAreaWidget->resize(this->width(), scrollAreaWidget->sizeHint().height());

    setWidget(scrollAreaWidget);
    setMinimumWidth(350);
}

void DefinitionScrollArea::resizeEvent(QResizeEvent *event) {
    scrollAreaWidget->resize(this->width(), scrollAreaWidget->sizeHint().height());
    event->accept();
}

DefinitionScrollArea::~DefinitionScrollArea()
{
    delete scrollAreaWidget;
}

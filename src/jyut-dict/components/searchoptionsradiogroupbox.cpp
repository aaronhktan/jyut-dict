#include "searchoptionsradiogroupbox.h"

#include <QLocale>

SearchOptionsRadioGroupBox::SearchOptionsRadioGroupBox(ISearchOptionsMediator *mediator,
                                                       QWidget *parent) :
    QGroupBox(parent)
{
    _mediator = mediator;

    setupUI();
    translateUI();
}

void SearchOptionsRadioGroupBox::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QGroupBox::changeEvent(event);
}

void SearchOptionsRadioGroupBox::setupUI()
{

    _layout = new QHBoxLayout{this};
    _layout->setContentsMargins(5, 5, 5, 5);

    _simplifiedButton = new QRadioButton{this};
    _traditionalButton = new QRadioButton{this};
    _jyutpingButton = new QRadioButton{this};
    _pinyinButton = new QRadioButton{this};
    _englishButton = new QRadioButton{this};
#ifdef Q_OS_LINUX
    _simplifiedButton->setStyleSheet("QToolTip { padding: 1px; color: black }");
    _traditionalButton->setStyleSheet("QToolTip { padding: 1px; color: black }");
    _jyutpingButton->setStyleSheet("QToolTip { padding: 1px; color: black }");
    _pinyinButton->setStyleSheet("QToolTip { padding: 1px; color: black; }");
    _englishButton->setStyleSheet("QToolTip { padding: 1px; color: black; }");
#else
    _simplifiedButton->setStyleSheet("QToolTip { padding: 1px; }");
    _traditionalButton->setStyleSheet("QToolTip { padding: 1px; }");
    _jyutpingButton->setStyleSheet("QToolTip { padding: 1px; }");
    _pinyinButton->setStyleSheet("QToolTip { padding: 1px; }");
    _englishButton->setStyleSheet("QToolTip { padding: 1px; }");
#endif
    _englishButton->click();
#ifdef Q_OS_MAC
    // Makes the button selection show up correctly on macOS
    _englishButton->setDown(true);
#endif
    notifyMediator();

    connect(_simplifiedButton, &QRadioButton::released, [this](){notifyMediator();});
    connect(_traditionalButton, &QRadioButton::released, [this](){notifyMediator();});
    connect(_jyutpingButton, &QRadioButton::released, [this](){notifyMediator();});
    connect(_pinyinButton, &QRadioButton::released, [this](){notifyMediator();});
    connect(_englishButton, &QRadioButton::released, [this](){notifyMediator();});

    _layout->addWidget(_simplifiedButton);
    _layout->addWidget(_traditionalButton);
    _layout->addWidget(_jyutpingButton);
    _layout->addWidget(_pinyinButton);
    _layout->addWidget(_englishButton);

    setLayout(_layout);
    setFlat(true);
#ifdef Q_OS_WIN
    if (QLocale::system().language() & QLocale::Chinese ||
        QLocale::system().language() & QLocale::Cantonese) {
        setStyleSheet("QRadioButton { font-size: 12px; }"
                      "QGroupBox { border: 0; }");
    }
#else
    setStyleSheet("QGroupBox { border: 0; }");
#endif
}

void SearchOptionsRadioGroupBox::translateUI()
{
    _simplifiedButton->setText(tr("SC"));
    _traditionalButton->setText(tr("TC"));
    _jyutpingButton->setText(tr("JP"));
    _pinyinButton->setText(tr("PY"));
    _englishButton->setText(tr("EN"));

    _simplifiedButton->setToolTip(tr("Search Simplified Chinese"));
    _traditionalButton->setToolTip(tr("Search Traditional Chinese"));
    _jyutpingButton->setToolTip(tr("Search Jyutping"));
    _pinyinButton->setToolTip(tr("Search Pinyin"));
    _englishButton->setToolTip(tr("Search English"));
}

void SearchOptionsRadioGroupBox::notifyMediator()
{
    if (_simplifiedButton->isChecked()) {
        _mediator->setParameters(SearchParameters::SIMPLIFIED);
    } else if (_traditionalButton->isChecked()) {
        _mediator->setParameters(SearchParameters::TRADITIONAL);
    } else if (_jyutpingButton->isChecked()) {
        _mediator->setParameters(SearchParameters::JYUTPING);
    } else if (_pinyinButton->isChecked()) {
        _mediator->setParameters(SearchParameters::PINYIN);
    } else if (_englishButton->isChecked()) {
        _mediator->setParameters(SearchParameters::ENGLISH);
    } else {
        _mediator->setParameters(SearchParameters::ENGLISH);
    }
}

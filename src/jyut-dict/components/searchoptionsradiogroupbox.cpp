#include "searchoptionsradiogroupbox.h"

SearchOptionsRadioGroupBox::SearchOptionsRadioGroupBox(ISearchOptionsMediator *mediator, QWidget *parent) :
    QGroupBox(parent)
{
    _mediator = mediator;

    _layout = new QHBoxLayout(this);
    _layout->setContentsMargins(5, 5, 5, 5);

    _simplifiedButton = new QRadioButton(tr("SC"), this);
    _simplifiedButton->setToolTip(tr("Search Simplified Chinese"));
    _traditionalButton = new QRadioButton(tr("TC"), this);
    _traditionalButton->setToolTip(tr("Search Traditional Chinese"));
    _jyutpingButton = new QRadioButton(tr("JP"), this);
    _jyutpingButton->setToolTip(tr("Search Jyutping"));
    _pinyinButton = new QRadioButton(tr("PY"), this);
    _pinyinButton->setToolTip(tr("Search Pinyin"));
    _englishButton = new QRadioButton(tr("EN"), this);
    _englishButton->setToolTip(tr("Search English"));
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
    _englishButton->setChecked(true);
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
    setStyleSheet("border: 0;");
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



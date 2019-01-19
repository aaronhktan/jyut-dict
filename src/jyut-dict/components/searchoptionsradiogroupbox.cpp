#include "searchoptionsradiogroupbox.h"

SearchOptionsRadioGroupBox::SearchOptionsRadioGroupBox(ISearchOptionsMediator *mediator, QWidget *parent) :
    QGroupBox(parent)
{
    _mediator = mediator;

    _layout = new QHBoxLayout(this);
    _layout->setContentsMargins(5, 5, 5, 5);

    _simplifiedButton = new QRadioButton(tr("SC"), this);
    _traditionalButton = new QRadioButton(tr("TC"), this);
    _jyutpingButton = new QRadioButton(tr("JP"), this);
    _pinyinButton = new QRadioButton(tr("PY"), this);
    _englishButton = new QRadioButton(tr("EN"), this);
    _englishButton->setChecked(true);
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



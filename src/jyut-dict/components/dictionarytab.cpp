#include "dictionarytab.h"

#include <QVBoxLayout>

DictionaryTab::DictionaryTab(QWidget *parent)
    : QWidget(parent)
{
    _tabLayout = new QVBoxLayout{this};

    _title = new QLabel{"hi!", this};

    _tabLayout->addWidget(_title);

    setLayout(_tabLayout);
}

DictionaryTab::DictionaryTab(QWidget *parent, QString text)
    : QWidget(parent)
{
    _tabLayout = new QVBoxLayout{this};

    _title = new QLabel{text, this};

    _tabLayout->addWidget(_title);

    setLayout(_tabLayout);
}

#include "dictionarytab.h"

#include "components/dictionarylistview.h"

#include <QVBoxLayout>

DictionaryTab::DictionaryTab(QWidget *parent)
    : QWidget(parent)
{
    _tabLayout = new QVBoxLayout{this};

    _title = new QLabel{"hi!", this};
    _list = new DictionaryListView{this};

    _tabLayout->addWidget(_title);
    _tabLayout->addWidget(_list);

    setLayout(_tabLayout);
}

DictionaryTab::DictionaryTab(QWidget *parent, QString text)
    : QWidget(parent)
{
    _tabLayout = new QVBoxLayout{this};

    _title = new QLabel{text, this};
    _list = new DictionaryListView{this};

    _tabLayout->addWidget(_title);
    _tabLayout->addWidget(_list);

    setLayout(_tabLayout);
}

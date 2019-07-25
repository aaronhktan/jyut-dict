#include "dictionarytab.h"

#include "components/dictionarylistview.h"
#include "logic/dictionary/dictionarymetadata.h"

#include <QGridLayout>
#include <QtSql>
#include <QVBoxLayout>

DictionaryTab::DictionaryTab(std::shared_ptr<SQLDatabaseManager> manager,
                             QWidget *parent)
    : QWidget(parent)
{
    _manager = manager;
    _tabLayout = new QGridLayout{this};

    _title = new QLabel{"hi!", this};
    _title->setWordWrap(true);
    _list = new DictionaryListView{this};
    _description = new QLabel{this};
    _description->setWordWrap(true);
    _legal = new QLabel{this};
    _legal->setWordWrap(true);

    _tabLayout->addWidget(_title, 1, 1, 1, -1);
    _tabLayout->addWidget(_list, 2, 1, 1, 1);
    _tabLayout->addWidget(_description, 2, 2, 1, 1);
    _tabLayout->addWidget(_legal, 3, 2, 1, 1);

    setLayout(_tabLayout);

//    connect(_list->selectionModel(),
//            &QItemSelectionModel::currentChanged,
//            this,
//            &DictionaryTab::setDictionaryMetadata);
//    connect(_list->selectionModel(),
//            SIGNAL(currentChanged(QModelIndex, QModelIndex)),
//            this, SLOT(handleSelectionChanged(QModelIndex)));
}

DictionaryTab::DictionaryTab(std::shared_ptr<SQLDatabaseManager> manager,
                             QString text,
                             QWidget *parent)
    : QWidget(parent)
{
    _manager = manager;
    _tabLayout = new QGridLayout{this};

    _title = new QLabel{text, this};
    _title->setWordWrap(true);
    _list = new DictionaryListView{this};
    _description = new QLabel{this};
    _description->setWordWrap(true);
    _legal = new QLabel{this};
    _legal->setWordWrap(true);

    _tabLayout->addWidget(_title, 1, 1, 1, -1);
    _tabLayout->addWidget(_list, 2, 1, 2, 1);
    _tabLayout->addWidget(_description, 2, 2, 1, 1);
    _tabLayout->addWidget(_legal, 3, 2, 1, 1);

    setLayout(_tabLayout);

    connect(_list->selectionModel(),
            &QItemSelectionModel::currentChanged,
            this,
            &DictionaryTab::setDictionaryMetadata);

    populateDictionaryList();
}

void DictionaryTab::setDictionaryMetadata(const QModelIndex &index)
{
    DictionaryMetadata metadata = qvariant_cast<DictionaryMetadata>(index.data());
    _title->setText(metadata.getName().c_str());
    _description->setText(metadata.getDescription().c_str());
    _legal->setText(metadata.getLegal().c_str());
}

void DictionaryTab::populateDictionaryList()
{
    QSqlQuery query{_manager->getEnglishDatabase()};
    query.prepare("SELECT sourcename, version, description, legal, link, other "
                  "FROM sources");

    query.exec();

    int sourcenameIndex = query.record().indexOf("sourcename");
    int versionIndex = query.record().indexOf("version");
    int descriptionIndex = query.record().indexOf("description");
    int legalIndex = query.record().indexOf("legal");
    int linkIndex = query.record().indexOf("link");
    int otherIndex = query.record().indexOf("other");
    while (query.next()) {
        std::string source
            = query.value(sourcenameIndex).toString().toStdString();
        std::string version
            = query.value(versionIndex).toString().toStdString();
        std::string description
            = query.value(descriptionIndex).toString().toStdString();
        std::string legal
            = query.value(legalIndex).toString().toStdString();
        std::string link
            = query.value(linkIndex).toString().toStdString();
        std::string other
            = query.value(otherIndex).toString().toStdString();
        qDebug() << description.c_str();

        DictionaryMetadata dictionary{source, version, description,
                                      legal, link, other};
        _list->model()->setData(QModelIndex(), QVariant::fromValue(dictionary));
    }
}

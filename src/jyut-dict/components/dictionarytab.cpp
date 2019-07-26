#include "dictionarytab.h"

#include "components/dictionarylistview.h"
#include "logic/dictionary/dictionarymetadata.h"

#include <QDesktopServices>
#include <QSpacerItem>
#include <QtSql>

DictionaryTab::DictionaryTab(std::shared_ptr<SQLDatabaseManager> manager,
                             QWidget *parent)
    : QWidget(parent)
{
    DictionaryTab(manager, "hi", parent);
}

DictionaryTab::DictionaryTab(std::shared_ptr<SQLDatabaseManager> manager,
                             QString text,
                             QWidget *parent)
    : QWidget(parent)
{
    _manager = manager;
    _tabLayout = new QGridLayout{this};
    _tabLayout->setAlignment(Qt::AlignTop);

    _title = new QLabel{text, this};
    _title->setWordWrap(true);
    _title->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    _list = new DictionaryListView{this};
    _groupbox = new QGroupBox{this};
    _description = new QLabel{this};
    _description->setWordWrap(true);
    _description->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    _legal = new QLabel{this};
    _legal->setWordWrap(true);
    _legal->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    _remove = new QPushButton{tr("Delete Dictionary"), this};
    _link = new QPushButton{tr("Website"), this};
    QSpacerItem *verticalSpacer = new QSpacerItem{0,
                                                  0,
                                                  QSizePolicy::Expanding,
                                                  QSizePolicy::Expanding};

    _tabLayout->addWidget(_title, 1, 1, 1, -1);
    _tabLayout->addWidget(_list, 2, 1, -1, 1);
    _tabLayout->addWidget(_groupbox, 2, 2, -1, 1);
    _groupboxLayout = new QGridLayout{_groupbox};
    _groupboxLayout->setAlignment(Qt::AlignTop);
    _groupboxLayout->addWidget(_description, 1, 1, 2, 3);
    _groupboxLayout->addWidget(_legal, 3, 1, 2, 3);
    _groupboxLayout->addItem(verticalSpacer, 5, 1, 1, -1);
    _groupboxLayout->addWidget(_remove, 9, 3, 1, 1);
    _groupboxLayout->setAlignment(_remove, Qt::AlignBottom);
    _groupboxLayout->addWidget(_link, 9, 2, 1, 1);
    _groupboxLayout->setAlignment(_link, Qt::AlignBottom);

    setLayout(_tabLayout);
    _groupbox->setLayout(_groupboxLayout);

    connect(_list->selectionModel(),
            &QItemSelectionModel::currentChanged,
            this,
            &DictionaryTab::setDictionaryMetadata);

    populateDictionaryList();
    _list->setCurrentIndex(_list->model()->index(0, 0));
}

void DictionaryTab::setDictionaryMetadata(const QModelIndex &index)
{
    DictionaryMetadata metadata = qvariant_cast<DictionaryMetadata>(
        index.data());
    _title->setText(metadata.getName().c_str());
    _description->setText(metadata.getDescription().c_str());
    _legal->setText(metadata.getLegal().c_str());

    disconnect(_link, nullptr, nullptr, nullptr);
    connect(_link, &QPushButton::clicked, this, [=] {
        QDesktopServices::openUrl(QUrl{metadata.getLink().c_str()});
    });

    if (_list->model()->rowCount() <= 1) {
        _remove->setEnabled(false);
    }
    disconnect(_remove, nullptr, nullptr, nullptr);
    connect(_remove, &QPushButton::clicked, this, [=] {
        QSqlQuery query{_manager->getEnglishDatabase()};
        query.prepare("PRAGMA foreign_keys = ON");
        query.exec();

        query.prepare("DELETE FROM sources WHERE sourcename = ?");
        query.addBindValue(metadata.getName().c_str());
        query.exec();

        query.prepare("DELETE FROM definitions_fts");
        query.exec();

        query.prepare("DELETE FROM entries_fts");
        query.exec();

        query.prepare("DELETE FROM entries WHERE entry_id NOT IN "
                      "(SELECT fk_entry_id FROM definitions)");
        query.exec();

        query.prepare("INSERT INTO entries_fts (rowid, pinyin, jyutping) "
                      "SELECT rowid, pinyin, jyutping FROM entries");
        query.exec();

        query.prepare("INSERT INTO definitions_fts (rowid, definition) "
                      "SELECT rowid, definition FROM definitions");
        query.exec();

        clearDictionaryList();
        populateDictionaryList();
        _list->setCurrentIndex(_list->model()->index(0, 0));
    });
}

void DictionaryTab::clearDictionaryList()
{
    _list->model()->removeRows(0, _list->model()->rowCount());
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

    int row = 0;
    while (query.next()) {
        row++;
        std::string source
            = query.value(sourcenameIndex).toString().toStdString();
        std::string version = query.value(versionIndex).toString().toStdString();
        std::string description
            = query.value(descriptionIndex).toString().toStdString();
        std::string legal = query.value(legalIndex).toString().toStdString();
        std::string link = query.value(linkIndex).toString().toStdString();
        std::string other = query.value(otherIndex).toString().toStdString();

        DictionaryMetadata dictionary{source,
                                      version,
                                      description,
                                      legal,
                                      link,
                                      other};
        _list->model()->setData(_list->model()->index(row, 0), QVariant::fromValue(dictionary));
    }
}

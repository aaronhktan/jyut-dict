#include "dictionarytab.h"

#include "components/dictionarylistview.h"
#include "logic/dictionary/dictionarymetadata.h"

#include <QDesktopServices>
#include <QFileDialog>
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
    : QWidget{parent},
    _manager{manager}
{
    _utils = new SQLDatabaseUtils{_manager};
    _tabLayout = new QGridLayout{this};
    _tabLayout->setAlignment(Qt::AlignTop);

    _explanatory = new QLabel{tr("The Dictionaries tab allows you to view "
                                 "information about dictionaries, "
                                 "and add or remove them."),
                              this};
    _explanatory->setWordWrap(true);
    _explanatory->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    _list = new DictionaryListView{this};
    _list->setFixedWidth(200);
    _groupbox = new QGroupBox{this};
    _groupbox->setMinimumWidth(350);
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
    _add = new QPushButton{tr("Add Dictionary..."), this};

    _tabLayout->addWidget(_explanatory, 1, 1, 1, -1);
    _tabLayout->addWidget(_list, 2, 1, 1, 1);
    _tabLayout->addWidget(_groupbox, 2, 2, 1, 1);
    _tabLayout->addWidget(_add, 3, 1, 1, 1);
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

    connect(_add, &QPushButton::clicked, this, [=] {
        QFileDialog *_fileDialog = new QFileDialog{this};
        _fileDialog->setFileMode(QFileDialog::ExistingFile);
        _fileDialog->setAcceptMode(QFileDialog::AcceptOpen);

        QString fileName = _fileDialog->getOpenFileName(
            this,
            tr("Select dictionary file"),
            "/Users/aaron/Documents/Github/jyut-dict/src/cedict_to_sqlite/",
            "Dictionary Files (*.db)");
        //        qDebug() << fileName;
        if (!fileName.toStdString().empty()) {
            addDictionary(fileName);
            clearDictionaryList();
            populateDictionaryList();
            _list->setCurrentIndex(_list->model()->index(0, 0));
        }
    });

    populateDictionaryList();
    _list->setCurrentIndex(_list->model()->index(0, 0));
}

void DictionaryTab::setDictionaryMetadata(const QModelIndex &index)
{
    DictionaryMetadata metadata = qvariant_cast<DictionaryMetadata>(
        index.data());
    _description->setText(metadata.getDescription().c_str());
    _legal->setText(metadata.getLegal().c_str());

    disconnect(_link, nullptr, nullptr, nullptr);
    connect(_link, &QPushButton::clicked, this, [=] {
        QDesktopServices::openUrl(QUrl{metadata.getLink().c_str()});
    });

    _remove->setEnabled(_list->model()->rowCount() > 1);
    disconnect(_remove, nullptr, nullptr, nullptr);
    connect(_remove, &QPushButton::clicked, this, [=] {
        removeDictionary(metadata);
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
        _list->model()->setData(_list->model()->index(row, 0),
                                QVariant::fromValue(dictionary));
    }
}

void DictionaryTab::addDictionary(QString &dictionaryFile)
{
    _utils->addSource(dictionaryFile.toStdString());
}

void DictionaryTab::removeDictionary(DictionaryMetadata metadata)
{
    _utils->removeSource(metadata.getName());
}

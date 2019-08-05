#include "dictionarytab.h"

#include "components/dictionarylistview.h"
#include "logic/dictionary/dictionarymetadata.h"
#include "logic/dictionary/dictionarysource.h"

#include <QtConcurrent/QtConcurrent>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QSpacerItem>
#include <QtSql>

DictionaryTab::DictionaryTab(std::shared_ptr<SQLDatabaseManager> manager,
                             QWidget *parent)
    : QWidget{parent},
    _manager{manager}
{
    _utils = std::make_unique<SQLDatabaseUtils>(_manager);

    setupUI();
    populateDictionaryList();
    _list->setCurrentIndex(_list->model()->index(0, 0));
}

void DictionaryTab::setupUI()
{
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
    _add = new QPushButton{tr("Add Dictionary..."), this};
    _groupbox = new QGroupBox{this};
    _groupbox->setMinimumWidth(350);

    _tabLayout->addWidget(_explanatory, 1, 1, 1, -1);
    _tabLayout->addWidget(_list, 2, 1, 1, 1);
    _tabLayout->addWidget(_groupbox, 2, 2, 1, 1);
    _tabLayout->addWidget(_add, 3, 1, 1, 1);

    setLayout(_tabLayout);

    _description = new QLabel{this};
    _description->setWordWrap(true);
    _description->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    _legal = new QLabel{this};
    _legal->setWordWrap(true);
    _legal->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    _version = new QLabel{this};
    _version->setWordWrap(true);
    _version->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    _remove = new QPushButton{tr("Delete Dictionary"), this};
    _link = new QPushButton{tr("Website"), this};

    _groupboxLayout = new QGridLayout{_groupbox};
    _groupboxLayout->addWidget(_description, 1, 1, 4, 3);
    _groupboxLayout->addWidget(_legal, 5, 1, 2, 3);
    _groupboxLayout->addWidget(_version, 7, 1, 1, 3);
    _groupboxLayout->setRowStretch(8, INT_MAX);
    _groupboxLayout->addWidget(_remove, 9, 3, 1, 1);
    _groupboxLayout->addWidget(_link, 9, 2, 1, 1);

    _groupbox->setLayout(_groupboxLayout);

    connect(_list->selectionModel(),
            &QItemSelectionModel::currentChanged,
            this,
            &DictionaryTab::setDictionaryMetadata);

    connect(_add, &QPushButton::clicked, this, [=] {
        QFileDialog *_fileDialog = new QFileDialog{this};
        _fileDialog->setFileMode(QFileDialog::ExistingFile);
        _fileDialog->setAcceptMode(QFileDialog::AcceptOpen);

        QString fileName = _fileDialog
                               ->getOpenFileName(this,
                                                 tr("Select dictionary file"),
                                                 QDir::homePath(),
                                                 "Dictionary Files (*.db)");
        if (!fileName.toStdString().empty()) {
            addDictionary(fileName);
        }
    });
}

void DictionaryTab::setDictionaryMetadata(const QModelIndex &index)
{
    DictionaryMetadata metadata = qvariant_cast<DictionaryMetadata>(
        index.data());
    _description->setText(metadata.getDescription().c_str());
    _legal->setText(metadata.getLegal().c_str());
    _version->setText((tr("Version: %1")).arg(metadata.getVersion().c_str()));

    disconnect(_link, nullptr, nullptr, nullptr);
    connect(_link, &QPushButton::clicked, this, [=] {
        QDesktopServices::openUrl(QUrl{metadata.getLink().c_str()});
    });

    _remove->setEnabled(_list->model()->rowCount() > 1);
    disconnect(_remove, nullptr, nullptr, nullptr);
    connect(_remove, &QPushButton::clicked, this, [=] {
        removeDictionary(metadata);
    });
}

void DictionaryTab::clearDictionaryList()
{
    _list->model()->removeRows(0, _list->model()->rowCount());
}

void DictionaryTab::populateDictionaryList()
{
    std::vector<DictionaryMetadata> sources;
    _utils->readSources(sources);

    for (std::vector<DictionaryMetadata>::size_type row = 0;
         row < sources.size();
         row++) {
        _list->model()->setData(_list->model()->index(static_cast<int>(row), 0),
                                QVariant::fromValue(sources.at(row)));
    };
}

void DictionaryTab::addDictionary(QString &dictionaryFile)
{
    QtConcurrent::run(_utils.get(),
                      &SQLDatabaseUtils::addSource,
                      dictionaryFile.toStdString());

    _dialog = new QProgressDialog{"", QString(), 0, 0, this};
    _dialog->setWindowModality(Qt::ApplicationModal);
    _dialog->setMinimumSize(300, 75);
    Qt::WindowFlags flags = _dialog->windowFlags() | Qt::CustomizeWindowHint;
    flags &= ~(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint
               | Qt::WindowFullscreenButtonHint);
    _dialog->setWindowFlags(flags);
    _dialog->setMinimumDuration(0);
    _dialog->setAttribute(Qt::WA_DeleteOnClose, true);

    _dialog->setLabelText(tr("Dropping search indexes..."));
    _dialog->setRange(0, 0);
    _dialog->setValue(0);

    disconnect(_utils.get(), nullptr, nullptr, nullptr);
    connect(_utils.get(), &SQLDatabaseUtils::insertingSource, this, [&] {
        _dialog->setLabelText(tr("Adding source..."));
    });

    connect(_utils.get(), &SQLDatabaseUtils::insertingEntries, this, [&] {
        _dialog->setLabelText(tr("Adding new entries..."));
    });

    connect(_utils.get(), &SQLDatabaseUtils::insertingDefinitions, this, [&] {
        _dialog->setLabelText(tr("Adding new definitions..."));
    });

    connect(_utils.get(), &SQLDatabaseUtils::rebuildingIndexes, this, [&] {
        _dialog->setLabelText(tr("Rebuilding search indexes..."));
    });

    connect(_utils.get(),
            &SQLDatabaseUtils::finishedAddition,
            this,
            [&](bool success, QString reason, QString description) {
                _dialog->reset();
                clearDictionaryList();
                populateDictionaryList();
                populateDictionarySourceUtils();
                _list->setCurrentIndex(_list->model()->index(0, 0));

                if (!success) {
                    failureMessage(reason, description);
                }
            });
}

void DictionaryTab::removeDictionary(DictionaryMetadata metadata)
{
    QtConcurrent::run(_utils.get(),
                      &SQLDatabaseUtils::removeSource,
                      metadata.getName());

    _dialog = new QProgressDialog{"", QString(), 0, 0, this};
    _dialog->setWindowModality(Qt::ApplicationModal);
    _dialog->setMinimumSize(300, 75);
    Qt::WindowFlags flags = _dialog->windowFlags() | Qt::CustomizeWindowHint;
    flags &= ~(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint
               | Qt::WindowFullscreenButtonHint);
    _dialog->setWindowFlags(flags);
    _dialog->setMinimumDuration(0);
    _dialog->setAttribute(Qt::WA_DeleteOnClose, true);

    _dialog->setLabelText(tr("Removing source..."));
    _dialog->setRange(0, 0);
    _dialog->setValue(0);

    disconnect(_utils.get(), nullptr, nullptr, nullptr);
    connect(_utils.get(), &SQLDatabaseUtils::deletingDefinitions, this, [&] {
        _dialog->setLabelText(tr("Removing definitions..."));
    });

    connect(_utils.get(),
            &SQLDatabaseUtils::totalToDelete,
            this,
            [&](int numToDelete) {
                _dialog->setRange(0, numToDelete + 1);
                _dialog->setLabelText(
                    QString{tr("Deleted definition 0 of %1")}.arg(numToDelete));
            });

    connect(_utils.get(),
            &SQLDatabaseUtils::deletionProgress,
            this,
            [&](int deleted, int total) {
                _dialog->setLabelText(
                    QString{tr("Deleted definition %1 of %2")}.arg(deleted).arg(
                        total));
                _dialog->setValue(deleted);
            });

    connect(_utils.get(), &SQLDatabaseUtils::rebuildingIndexes, this, [&] {
        _dialog->setLabelText(tr("Rebuilding search indexes..."));
        _dialog->setRange(0, 0);
    });

    connect(_utils.get(), &SQLDatabaseUtils::rebuildingIndexes, this, [&] {
        _dialog->setLabelText(tr("Cleaning up..."));
    });

    connect(_utils.get(),
            &SQLDatabaseUtils::finishedDeletion,
            this,
            [&](bool success) {
                _dialog->setLabelText(success ? tr("Done!") : tr("Failed!"));

                if (success) {
                    std::vector<std::pair<std::string, std::string>> sources;
                    _utils->readSources(sources);
                    for (auto source : sources) {
                        DictionarySourceUtils::addSource(source.first, source.second);
                    }
                }

                QTimer::singleShot(500, [&] {
                    _dialog->reset();
                    clearDictionaryList();
                    populateDictionaryList();
                    _list->setCurrentIndex(_list->model()->index(0, 0));
                });
            });
}

void DictionaryTab::populateDictionarySourceUtils()
{
    std::vector<std::pair<std::string, std::string>> sources;
    _utils->readSources(sources);
    for (auto source : sources) {
        DictionarySourceUtils::addSource(source.first,
                                         source.second);
    }
}

void DictionaryTab::failureMessage(QString reason, QString description)
{
    _message = new QMessageBox{this};
    Qt::WindowFlags flags = _message->windowFlags()
                            | Qt::CustomizeWindowHint;
    flags &= ~(Qt::WindowMinMaxButtonsHint
               | Qt::WindowCloseButtonHint
               | Qt::WindowFullscreenButtonHint);
    _message->setWindowFlags(flags);
    _message->setAttribute(Qt::WA_DeleteOnClose, true);
    _message->setText(tr("Failed to add source!"));
    _message->setInformativeText(reason);
    _message->setDetailedText(description);
    _message->setIcon(QMessageBox::Warning);

    // setDefaultButton doesn't really work, so use this
    // workaround to deselect all buttons first.
    for (auto button : _message->buttons()) {
        QPushButton *b = static_cast<QPushButton *>(button);
        b->setDown(false);
        b->setAutoDefault(false);
        b->setDefault(false);
    }
    _message->setDefaultButton(QMessageBox::Ok);

    // Setting minimum width also doesn't work, so use this
    // workaround to set a width.
    QSpacerItem *horizontalSpacer
        = new QSpacerItem(400,
                          0,
                          QSizePolicy::Minimum,
                          QSizePolicy::Minimum);
    QGridLayout *layout = static_cast<QGridLayout *>(
        _message->layout());
    layout->addItem(horizontalSpacer,
                    layout->rowCount(),
                    0,
                    1,
                    layout->columnCount());

    _message->exec();
}

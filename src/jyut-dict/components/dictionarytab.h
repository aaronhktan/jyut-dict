#ifndef DICTIONARYTAB_H
#define DICTIONARYTAB_H

#include "logic/database/sqldatabasemanager.h"
#include "logic/database/sqldatabaseutils.h"
#include "logic/dictionary/dictionarymetadata.h"

#include <QLabel>
#include <QGridLayout>
#include <QGroupBox>
#include <QListView>
#include <QMessageBox>
#include <QModelIndex>
#include <QProgressDialog>
#include <QPushButton>
#include <QWidget>

#include <memory>

class DictionaryTab : public QWidget
{
    Q_OBJECT

public:
    explicit DictionaryTab(std::shared_ptr<SQLDatabaseManager> manager,
                           QWidget *parent = nullptr);

private:
    void setupUI();

    void setDictionaryMetadata(const QModelIndex &index);

    void clearDictionaryList();
    void populateDictionaryList();
    void addDictionary(QString &dictionaryFile);
    void removeDictionary(DictionaryMetadata metadata);
    void populateDictionarySourceUtils();

    void failureMessage(QString reason, QString description);

    QLabel *_explanatory;
    QLabel *_description;
    QLabel *_legal;
    QLabel *_version;

    QPushButton *_add;
    QPushButton *_remove;
    QPushButton *_link;

    QGridLayout *_tabLayout;
    QGridLayout *_groupboxLayout;

    QListView *_list;
    QGroupBox *_groupbox;

    QProgressDialog *_dialog;
    QMessageBox *_message;

    std::shared_ptr<SQLDatabaseManager> _manager;
    std::unique_ptr<SQLDatabaseUtils> _utils;
signals:

public slots:
};

#endif // DICTIONARYTAB_H
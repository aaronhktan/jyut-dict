#ifndef DICTIONARYTAB_H
#define DICTIONARYTAB_H

#include "logic/dictionary/dictionarymetadata.h"
#include "logic/search/sqldatabasemanager.h"

#include <QLabel>
#include <QGridLayout>
#include <QGroupBox>
#include <QListView>
#include <QModelIndex>
#include <QPushButton>
#include <QWidget>

#include <memory>

class DictionaryTab : public QWidget
{
    Q_OBJECT

public:
    explicit DictionaryTab(std::shared_ptr<SQLDatabaseManager> manager,
                           QWidget *parent = nullptr);
    explicit DictionaryTab(std::shared_ptr<SQLDatabaseManager> manager,
                           QString text = "hi",
                           QWidget *parent = nullptr);

    void setDictionaryMetadata(const QModelIndex &index);

private:
    void clearDictionaryList();
    void populateDictionaryList();

    QLabel *_title;
    QLabel *_description;
    QLabel *_legal;

    QPushButton *_remove;
    QPushButton *_link;

    QGridLayout *_tabLayout;
    QGridLayout *_groupboxLayout;

    QListView *_list;
    QGroupBox *_groupbox;


    std::shared_ptr<SQLDatabaseManager> _manager;

signals:

public slots:
};

#endif // DICTIONARYTAB_H

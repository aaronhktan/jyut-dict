#ifndef DICTIONARYTAB_H
#define DICTIONARYTAB_H

#include "dialogs/dictionarytabfailuredialog.h"
#include "dialogs/overwriteconflictingdictionarydialog.h"
#include "logic/database/sqldatabasemanager.h"
#include "logic/database/sqldatabaseutils.h"
#include "logic/dictionary/dictionarymetadata.h"

#include <QEvent>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QListView>
#include <QModelIndex>
#include <QProgressDialog>
#include <QPushButton>
#include <QWidget>

#include <memory>

// The DictionaryTab is the widget displayed in the settings window
// when a user clicks on the "Dictionaries" icon in the toolbar.

class DictionaryTab : public QWidget
{
    Q_OBJECT

public:
    explicit DictionaryTab(std::shared_ptr<SQLDatabaseManager> manager,
                           QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

private:
    void setupUI();
    void translateUI();

    void setStyle(bool use_dark);

    void setDictionaryMetadata(const QModelIndex &index);

    void clearDictionaryList();
    void populateDictionaryList();
    void addDictionary(const QString &dictionaryFile);
    void forceAddDictionary(const QString &dictionaryFile);
    void removeDictionary(DictionaryMetadata metadata);
    void populateDictionarySourceUtils() const;

    void failureMessage(const QString &reason, const QString &description);

    QLabel *_explanatory;
    QLabel *_description;
    QLabel *_legal;
    QLabel *_version;

    QPushButton *_add;
    QPushButton *_findMore;
    QPushButton *_remove;
    QPushButton *_link;

    QGridLayout *_tabLayout;
    QGridLayout *_groupboxLayout;

    QListView *_list;
    QGroupBox *_groupbox;

    QProgressDialog *_dialog;
    OverwriteConflictingDictionaryDialog *_overwriteDialog;
    DictionaryTabFailureDialog *_message;

    std::shared_ptr<SQLDatabaseManager> _manager;
    std::unique_ptr<SQLDatabaseUtils> _utils;
};

#endif // DICTIONARYTAB_H

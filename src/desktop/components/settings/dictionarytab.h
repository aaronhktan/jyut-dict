#ifndef DICTIONARYTAB_H
#define DICTIONARYTAB_H

#include <QWidget>

#include <memory>

class DictionaryTabFailureDialog;
class OverwriteConflictingDictionaryDialog;
class SQLDatabaseManager;
class SQLDatabaseUtils;
class SourceMetadata;

class QEvent;
class QGridLayout;
class QGroupBox;
class QLabel;
class QListView;
class QModelIndex;
class QProgressDialog;
class QPushButton;

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

    void setSourceMetadata(const QModelIndex &index);

    void clearDictionaryList();
    void populateDictionaryList();
    void addDictionary(const QString &dictionaryFile);
    void forceAddDictionary(const QString &dictionaryFile);
    void removeDictionary(SourceMetadata metadata);
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

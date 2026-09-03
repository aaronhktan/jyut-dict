#ifndef RELATEDBUTTONCONTENTWIDGET_H
#define RELATEDBUTTONCONTENTWIDGET_H

#include "logic/search/searchparameters.h"

#include <QSettings>
#include <QWidget>

class Entry;

class QEvent;
class QLabel;
class QPushButton;
class QVBoxLayout;

enum class RelatedType {
    SearchBeginning,
    SearchContaining,
    SearchEnding,
};

class RelatedButtonContentWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RelatedButtonContentWidget(RelatedType type,
                                        QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

    void setEntry(const Entry &entry);

private:
    void setupUI();
    void translateUI();

    void setStyle(bool use_dark);

    bool _paletteRecentlyChanged = false;

    std::unique_ptr<QSettings> _settings;

    RelatedType _buttonType;

    std::string _traditional;
    std::string _simplified;

    QVBoxLayout *_relatedButtonLayout;
    QLabel *_description;
    QPushButton *_actionButton;

signals:
    void searchQuery(const QString &query, const SearchParameters &parameters);

public slots:
    void searchRequested(void);

private slots:
    void searchQueryRequested(void);
};

#endif // RELATEDBUTTONCONTENTWIDGET_H

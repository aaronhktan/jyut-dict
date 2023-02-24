#ifndef RELATEDBUTTONCONTENTWIDGET_H
#define RELATEDBUTTONCONTENTWIDGET_H

#include "logic/entry/entry.h"
#include "logic/search/searchparameters.h"

#include <QEvent>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>
#include <QWidget>

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

private slots:
    void searchQueryRequested(void);
};

#endif // RELATEDBUTTONCONTENTWIDGET_H

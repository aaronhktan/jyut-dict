#ifndef RELATEDBUTTON_H
#define RELATEDBUTTON_H

#include "components/related/relatedbuttoncontentwidget.h"
#include "components/related/relatedbuttonheaderwidget.h"
#include "logic/entry/entry.h"
#include "logic/search/searchparameters.h"

#include <QEvent>
#include <QVBoxLayout>
#include <QWidget>

class RelatedButton : public QWidget
{
    Q_OBJECT
public:
    explicit RelatedButton(RelatedType type, QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

    void setEntry(const Entry &entry);

private:
    void setupUI();

    void setStyle(bool use_dark);

    bool _paletteRecentlyChanged = false;

    RelatedType _buttonType;

    std::string _traditional;
    std::string _simplified;

    QVBoxLayout *_relatedButtonLayout;
    RelatedButtonHeaderWidget *_header;
    RelatedButtonContentWidget *_content;

signals:
    void search(void);

    void searchQuery(QString query, SearchParameters parameters);

public slots:
    void updateStyleRequested(void);

    void searchRequested(void);

    void searchQueryRequested(QString query, SearchParameters parameters);
};

#endif // RELATEDBUTTON_H

#ifndef SEARCHOPTIONSRADIOGROUPBOX_H
#define SEARCHOPTIONSRADIOGROUPBOX_H

#include "logic/analytics/analytics.h"
#include "logic/search/isearchoptionsmediator.h"
#include "logic/search/searchparameters.h"
#include "logic/utils/utils.h"

#include <QEvent>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QWidget>

// The SearchOptionsRadioGroupBox allows users to change search parameters
// Currently, there are five options for searching, more may be added

class SearchOptionsRadioGroupBox : public QGroupBox
{
Q_OBJECT

public:
    explicit SearchOptionsRadioGroupBox(ISearchOptionsMediator *mediator,
                                        QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

    void setOption(const Utils::ButtonOptionIndex index);
    void setOption(const SearchParameters parameters);

private:
    void setupUI();
    void translateUI();

    void notifyMediator();

    Analytics *_analytics;

    ISearchOptionsMediator *_mediator;

    QHBoxLayout *_layout;

    QRadioButton *_simplifiedButton;
    QRadioButton *_traditionalButton;
    QRadioButton *_jyutpingButton;
    QRadioButton *_pinyinButton;
    QRadioButton *_englishButton;
};

#endif // SEARCHOPTIONSRADIOGROUPBOX_H

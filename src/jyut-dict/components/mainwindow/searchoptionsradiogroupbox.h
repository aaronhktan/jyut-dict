#ifndef SEARCHOPTIONSRADIOGROUPBOX_H
#define SEARCHOPTIONSRADIOGROUPBOX_H

#include "logic/search/isearchoptionsmediator.h"
#include "logic/search/searchparameters.h"
#include "logic/utils/utils.h"

#include <QEvent>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
//#include <QRadioButton>
#include <QPushButton>
#include <QSettings>
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
    void setStyle(bool use_dark);

    void notifyMediator() const;

    bool _paletteRecentlyChanged = false;

    ISearchOptionsMediator *_mediator;
    std::unique_ptr<QSettings> _settings;

    QHBoxLayout *_layout;

    QLabel *_currentChoiceLabel;
    QPushButton *_simplifiedButton;
    QPushButton *_traditionalButton;
    QPushButton *_jyutpingButton;
    QPushButton *_pinyinButton;
    QPushButton *_englishButton;
};

#endif // SEARCHOPTIONSRADIOGROUPBOX_H

#ifndef SEARCHOPTIONSRADIOGROUPBOX_H
#define SEARCHOPTIONSRADIOGROUPBOX_H

#include "isearchoptionsselector.h"
#include "logic/search/isearchobserver.h"
#include "logic/search/isearchoptionsmediator.h"
#include "logic/search/searchparameters.h"

#include <QGroupBox>
#include <QSettings>

class SQLSearch;

class QEvent;
class QHBoxLayout;
class QLabel;
class QPushButton;
class QWidget;

// The SearchOptionsRadioGroupBox allows users to change search parameters
// Currently, there are five options for searching, more may be added

class SearchOptionsRadioGroupBox : public QGroupBox,
                                   public ISearchObserver,
                                   public ISearchOptionsSelector
{
Q_OBJECT

public:
    explicit SearchOptionsRadioGroupBox(
        std::shared_ptr<ISearchOptionsMediator> mediator,
        std::shared_ptr<SQLSearch> manager,
        QWidget *parent = nullptr);

    void detectedLanguage(SearchParameters) override;
    void changeEvent(QEvent *event) override;

    void setOption(const SearchParameters parameters) override;

private:
    void setupUI(void);
    void translateUI(void);
    void setStyle(bool use_dark);

    void notifyMediator(void);

    bool _paletteRecentlyChanged = false;

    std::shared_ptr<ISearchOptionsMediator> _mediator;
    std::shared_ptr<SQLSearch> _search;
    std::unique_ptr<QSettings> _settings;

    QHBoxLayout *_layout;

    QLabel *_currentChoiceLabel;
    QPushButton *_simplifiedButton;
    QPushButton *_traditionalButton;
    QPushButton *_jyutpingButton;
    QPushButton *_pinyinButton;
    QPushButton *_englishButton;

signals:
    void optionSelected(const SearchParameters parameters) override;
};

#endif // SEARCHOPTIONSRADIOGROUPBOX_H

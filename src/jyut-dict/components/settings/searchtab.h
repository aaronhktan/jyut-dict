#ifndef SEARCHTAB_H
#define SEARCHTAB_H

#include <QCheckBox>
#include <QComboBox>
#include <QEvent>
#include <QFormLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSettings>
#include <QWidget>

#include <memory>

// The search settings tab displays settings related to searching.

class SearchTab : public QWidget
{
    Q_OBJECT

public:
    explicit SearchTab(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

private:
    void setupUI();
    void translateUI();
    void setStyle(bool use_dark);

    void initializeSearchAutoDetectCheckbox(QCheckBox &searchAutoDetectCheckbox);
    void initializeFuzzyJyutping(QWidget &widget);
    void initializeDangerousFuzzyJyutping(QWidget &widget);
    void initializeFuzzyPinyin(QWidget &widget);

    void setSearchAutoDetectCheckboxDefault(QCheckBox &searchAutoDetectCheckbox);
    void setFuzzyJyutpingCheckboxDefault(QCheckBox &fuzzyJyutpingCheckbox);
    void setDangerousFuzzyJyutpingCheckboxDefault(
        QCheckBox &dangerouFuzzyJyutpingCheckbox);
    void setFuzzyPinyinCheckboxDefault(QCheckBox &fuzzyPinyinCheckbox);

    bool _paletteRecentlyChanged = false;

    std::unique_ptr<QSettings> _settings;

    QCheckBox *_searchAutoDetectCheckbox;

    QLabel *_jyutpingTitleLabel;
    QWidget *_fuzzyJyutping;
    QGridLayout *_fuzzyJyutpingLayout;
    QCheckBox *_fuzzyJyutpingCheckbox;
    QLabel *_fuzzyJyutpingDescription;

    QWidget *_dangerousFuzzyJyutping;
    QGridLayout *_dangerousFuzzyJyutpingLayout;
    QCheckBox *_dangerousFuzzyJyutpingCheckbox;
    QLabel *_dangerousFuzzyJyutpingDescription;

    QLabel *_pinyinTitleLabel;
    QWidget *_fuzzyPinyin;
    QGridLayout *_fuzzyPinyinLayout;
    QCheckBox *_fuzzyPinyinCheckbox;
    QLabel *_fuzzyPinyinDescription;

    QFormLayout *_tabLayout;

signals:
    void triggerSearch(void);

public slots:
    void resetSettings(void);
};

#endif // SEARCHTAB_H

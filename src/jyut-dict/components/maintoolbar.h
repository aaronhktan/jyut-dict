#ifndef MAINTOOLBAR_H
#define MAINTOOLBAR_H

#include "components/searchlineedit.h"

#include <QHBoxLayout>
#include <QToolBar>

class MainToolBar : public QToolBar
{
public:
    explicit MainToolBar(QWidget *parent = nullptr);
    ~MainToolBar();

private:
    QHBoxLayout *toolBarLayout;
    QWidget *toolBarWidget;
    SearchLineEdit *searchBar;
};

#endif // MAINTOOLBAR_H

#include "windows/mainwindow.h"

#include "logic/search/sqldatabasemanager.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setAttribute(Qt::AA_EnableHighDpiScaling);
    a.setAttribute(Qt::AA_UseHighDpiPixmaps);

    SQLDatabaseManager manager(":/test.db");

    MainWindow w;
    w.show();

    return a.exec();
}
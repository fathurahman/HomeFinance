#include "mainwindow.h"
#include "sqlite/sqlite3.h"

#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    sqlite3 *db;
    int rc = sqlite3_open("ucup.db", &db);
    if (rc)
    {
        qWarning() << "Si ucup error nih mil: " << sqlite3_errmsg(db);
    }
    else
    {
        qDebug() << "Open, but immediately close it";
        sqlite3_close(db);
    }

}

MainWindow::~MainWindow()
{
}


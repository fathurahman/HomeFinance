#include "mainwindow.h"
#include "database.h"
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    auto* db = new Database(this);
    /*
    int wi = db->addWallet("cash", 0);
    auto& w = db->wallet(wi);
    Journal j;
    j.timestamp = QDateTime::currentDateTime();
    j.title = "test";
    j.isDebit = true;
    j.addEntry("bca", 1, 1500000);
    db->addJournal(w.index, j);
    j.title = "belanja";
    j.entries.clear();
    j.isDebit = false;
    j.addEntry("gorengan", 4, 5000);
    j.addEntry("sampeu", 1, 32500);
    j.addEntry("sania", 1, 18250);
    db->addJournal(w.index, j);
    w.debug();
    db->save("D:/ucup.json");
    */
    db->load("D:/ucup.json");
    auto& w = db->wallet(0);
    Journal j;
    j.title = "belanja 2";
    j.timestamp = QDateTime::currentDateTime();
    j.entries.clear();
    j.isDebit = false;
    j.addEntry("gorengan", 10, 10000);
    j.addEntry("bimoli", 2, 13000);
    j.addEntry("rtx 4090", 1, 350000);
    db->addJournal(w.index, j);
    w.debug();
    qDebug() << db->journalEntryNames();

}

MainWindow::~MainWindow()
{
}


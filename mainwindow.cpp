#include "mainwindow.h"
#include <QDebug>
#include <QSettings>
#include <QFileDialog>
#include <QApplication>
#include <QMenuBar>
#include <QToolBar>
#include <QFile>
#include "application.h"
#include "database.h"
#include "addwalletdialog.h"
#include "addjournaldialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    updateWindowTitle();
    setMinimumSize(800, 600);
    createActions();
    createMenus();

}

MainWindow::~MainWindow()
{
}

void MainWindow::updateWindowTitle()
{
    auto str = QString("Home Finance Rp. %1").arg(db->totalValue());
    setWindowTitle(str);
}

void MainWindow::open()
{
    QSettings s;
    QString filePath = s.value("LastFilePath", "").toString();
    filePath = QFileDialog::getOpenFileName(this, "Open Database File", filePath, "HFDB (*.hfdb)");
    if (!filePath.isEmpty())
    {
        s.setValue("LastFilePath", filePath);
        db->load(filePath);
        m_actSave->setEnabled(true);
        m_actSaveAs->setEnabled(true);
    }
}

void MainWindow::save()
{
    QSettings s;
    QString filePath = s.value("LastFilePath", "").toString();
    if (filePath.isEmpty())
    {
        filePath = QFileDialog::getSaveFileName(this, "Save Database File", "", "HFDB (*.hfdb)");
        if (filePath.isEmpty())
            return;
    }
    db->save(filePath);
}

void MainWindow::saveAs()
{
    QSettings s;
    QString filePath = s.value("LastFilePath", "").toString();
    filePath = QFileDialog::getSaveFileName(this, "Save Database File As", filePath, "HFDB (*.hfdb)");
    if (!filePath.isEmpty())
    {
        s.setValue("LastFilePath", filePath);
        db->save(filePath);
    }
}

void MainWindow::quit()
{
    qApp->quit();
}

void MainWindow::addWallet()
{
    AddWalletDialog d(this);
    if (d.exec()) {
        db->walletDataList.append(d.walletData());
        db->activeWallet = db->walletDataList.size() - 1;
        updateWindowTitle();
        m_actAddDebitJournal->setEnabled(true);
        m_actAddCreditJournal->setEnabled(true);
    }
}

void MainWindow::addDebitJournal()
{
    AddJournalDialog d(true, this);
    if (d.exec()) {
        auto j = d.journalData();
        if (j.entryDataList.size() > 0) {
            db->journalDataList.append(d.journalData());
        }
    }
}

void MainWindow::addCreditJournal()
{
    AddJournalDialog d(false, this);
    if (d.exec())
    {
        db->journalDataList.append(d.journalData());
    }
}

void MainWindow::createActions()
{
    m_actOpen = new QAction("&Open", this);
    m_actOpen->setShortcuts(QKeySequence::Open);
    m_actOpen->setStatusTip("Open database file");
    connect(m_actOpen, &QAction::triggered, this, &MainWindow::open);

    m_actSave = new QAction("&Save", this);
    m_actSave->setShortcuts(QKeySequence::Save);
    m_actSave->setStatusTip("Save database file");
    connect(m_actSave, &QAction::triggered, this, &MainWindow::save);

    m_actSaveAs = new QAction("Save &As", this);
    m_actSaveAs->setShortcuts(QKeySequence::SaveAs);
    m_actSaveAs->setStatusTip("Save database file as");
    connect(m_actSaveAs, &QAction::triggered, this, &MainWindow::saveAs);

    m_actQuit = new QAction("&Quit", this);
    m_actQuit->setShortcuts(QKeySequence::Quit);
    m_actQuit->setStatusTip("Quit Home Finance");
    connect(m_actQuit, &QAction::triggered, this, &MainWindow::quit);

    m_actAddWallet = new QAction("Add &Wallet", this);
    m_actAddWallet->setShortcuts(QKeySequence::SelectAll);
    m_actAddWallet->setStatusTip("Add new wallet");
    connect(m_actAddWallet, &QAction::triggered, this, &MainWindow::addWallet);

    const bool hasWallet = db->walletDataList.size() > 0;

    m_actAddDebitJournal = new QAction("Add &Debit Journal", this);
    m_actAddDebitJournal->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_D));
    m_actAddDebitJournal->setStatusTip("Add debit journal to the current wallet");
    m_actAddDebitJournal->setEnabled(hasWallet);
    connect(m_actAddDebitJournal, &QAction::triggered, this, &MainWindow::addDebitJournal);

    m_actAddCreditJournal = new QAction("Add &Credit Journal", this);
    m_actAddCreditJournal->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_C));
    m_actAddCreditJournal->setStatusTip("Add credit journal to the current wallet");
    m_actAddCreditJournal->setEnabled(hasWallet);
    connect(m_actAddCreditJournal, &QAction::triggered, this, &MainWindow::addCreditJournal);

}

void MainWindow::createMenus()
{
    auto mb = menuBar();

    m_fileMenu = mb->addMenu("&File");
    m_fileMenu->addAction(m_actOpen);
    m_fileMenu->addAction(m_actSave);
    m_fileMenu->addAction(m_actSaveAs);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_actQuit);

    m_databaseMenu = mb->addMenu("Database");
    m_databaseMenu->addAction(m_actAddWallet);
    m_databaseMenu->addSeparator();
    m_databaseMenu->addAction(m_actAddDebitJournal);
    m_databaseMenu->addAction(m_actAddCreditJournal);

}


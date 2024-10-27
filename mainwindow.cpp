#include "mainwindow.h"
#include <QDebug>
#include <QSettings>
#include <QFileDialog>
#include <QApplication>
#include <QMenuBar>
#include <QToolBar>
#include <QFile>
#include <QTableView>
#include <QCloseEvent>
#include <QMessageBox>
#include "application.h"
#include "database.h"
#include "addwalletdialog.h"
#include "addjournaldialog.h"
#include "viewwalletsdialog.h"
#include "transactiontablemodel.h"
#include "transactiontableview.h"
#include "filterwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    updateWindowTitle();
    setMinimumSize(1024, 720);
    createActions();
    createMenus();
    createCentralWidget();
    connect(db, &Database::totalValueChanged, this, &MainWindow::updateWindowTitle);
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QString path = db->lastFilePath();
    if (db->isModified() && path.isEmpty() == false)
    {
        QMessageBox mb;
        mb.setText("The database has been modified.");
        mb.setInformativeText("Do yout want to save your changes ?");
        mb.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        mb.setDefaultButton(QMessageBox::Save);
        int ret = mb.exec();
        switch(ret)
        {
        case QMessageBox::Save:
            db->save(path);
            break;
        case QMessageBox::Discard:
            event->accept();
            break;
        default:
            event->ignore();
            break;
        }
    }
}

void MainWindow::updateWindowTitle()
{
	qlonglong val = db->totalValue();
    auto str = QString("Home Finance Rp. %L1").arg(val);
    setWindowTitle(str);
}

void MainWindow::open()
{
    QSettings s;
    QString filePath = s.value("LastFilePath", "").toString();
    filePath = QFileDialog::getOpenFileName(this, "Open Database File", filePath, QString("HF (*%1)").arg(FILE_EXT));
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
    QString path = db->lastFilePath();
    if (path.isEmpty())
    {
        path = QFileDialog::getSaveFileName(this, "Save Database File", "", QString("HF (*%1)").arg(FILE_EXT));
        if (!path.isEmpty())
        {
            db->save(path);
            QSettings s;
            s.setValue("LastFilePath", path);
            s.sync();
        }
    }
    else
    {
        db->save(path);
    }
}

void MainWindow::saveAs()
{
    QString path = db->lastFilePath();
    path = QFileDialog::getSaveFileName(this, "Save Database File As", path, QString("HF (*.%1)").arg(FILE_EXT));
    if (false == path.isEmpty())
    {
        if (db->save(path))
        {
            QSettings s;
            s.setValue("LastFilePath", path);
            s.sync();
        }
    }
}

void MainWindow::quit()
{
    qApp->quit();
}

void MainWindow::addWallet()
{
    AddWalletDialog d(this);
    if (d.exec())
    {
        db->addWallet(d.wallet());
    }
}

void MainWindow::addDebitJournal()
{
    AddJournalDialog d(true, this);
    if (d.exec())
    {
        db->addJournal(d.journalForm());
    }
}

void MainWindow::addCreditJournal()
{
    AddJournalDialog d(false, this);
    if (d.exec())
    {
        db->addJournal(d.journalForm());
    }
}

void MainWindow::viewWallets()
{
    ViewWalletsDialog d(this);
    d.exec();
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

    m_actAddDebitJournal = new QAction("Add &Debit Journal", this);
    m_actAddDebitJournal->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_D));
    m_actAddDebitJournal->setStatusTip("Add debit journal to the current wallet");
    connect(m_actAddDebitJournal, &QAction::triggered, this, &MainWindow::addDebitJournal);

    m_actAddCreditJournal = new QAction("Add &Credit Journal", this);
    m_actAddCreditJournal->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_C));
    m_actAddCreditJournal->setStatusTip("Add credit journal to the current wallet");
    connect(m_actAddCreditJournal, &QAction::triggered, this, &MainWindow::addCreditJournal);

    m_actViewWallets = new QAction("View Wallets", this);
    m_actViewWallets->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_W));
    m_actViewWallets->setStatusTip("View all wallets in the database");
    connect(m_actViewWallets, &QAction::triggered, this, &MainWindow::viewWallets);
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
    m_databaseMenu->addAction(m_actAddDebitJournal);
    m_databaseMenu->addAction(m_actAddCreditJournal);
    m_databaseMenu->addSeparator();
    m_databaseMenu->addAction(m_actViewWallets);
}

void MainWindow::createCentralWidget()
{
    auto* filter = new FilterWidget;
    auto* table = new TransactionTableView;

    auto* layout = new QVBoxLayout;
    layout->addWidget(filter);
    layout->addWidget(table);

    auto* widget = new QWidget;
    widget->setLayout(layout);

    setCentralWidget(widget);

}

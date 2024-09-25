#include "mainwindow.h"
#include "database.h"
#include <QDebug>
#include <QSettings>
#include <QFileDialog>
#include <QApplication>
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_db = new Database(this);
    createActions();
    createMenus();
}

MainWindow::~MainWindow()
{
}

void MainWindow::open()
{
    QSettings s;
    QString filePath = s.value("LastFilePath", "").toString();
    filePath = QFileDialog::getOpenFileName(this, "Open Database File", filePath, "JSON (*.json)");
    if (!filePath.isEmpty())
    {
        s.setValue("LastFilePath", filePath);
        m_db->load(filePath);
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
        filePath = QFileDialog::getSaveFileName(this, "Save Database File", "", "JSON (*.json)");
        if (filePath.isEmpty())
            return;
    }
    m_db->save(filePath);
}

void MainWindow::saveAs()
{
    QSettings s;
    QString filePath = s.value("LastFilePath", "").toString();
    filePath = QFileDialog::getSaveFileName(this, "Save Database File As", filePath, "JSON (*.json)");
    if (!filePath.isEmpty())
    {
        s.setValue("LastFilePath", filePath);
        m_db->save(filePath);
    }
}

void MainWindow::quit()
{
    qApp->quit();
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
}


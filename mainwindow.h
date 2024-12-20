﻿#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QAction* m_actOpen;
    QAction* m_actSave;
    QAction* m_actSaveAs;
    QAction* m_actQuit;
    QAction* m_actAddWallet;
    QAction* m_actAddDebitJournal;
    QAction* m_actAddCreditJournal;
    QAction* m_actViewWallets;
    QAction* m_actSortTransactions;

    QMenu* m_fileMenu;
    QMenu* m_databaseMenu;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void updateWindowTitle();
    void open();
    void save();
    void saveAs();
    void quit();
    void addWallet();
    void addDebitJournal();
    void addCreditJournal();
    void viewWallets();

private:
    void createActions();
    void createMenus();
    void createCentralWidget();


};
#endif // MAINWINDOW_H

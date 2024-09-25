#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include "database.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Database *m_db;

    QAction* m_actOpen;
    QAction* m_actSave;
    QAction* m_actSaveAs;
    QAction* m_actQuit;

    QMenu* m_fileMenu;

private slots:
    void open();
    void save();
    void saveAs();
    void quit();

private:
    void createActions();
    void createMenus();


};
#endif // MAINWINDOW_H

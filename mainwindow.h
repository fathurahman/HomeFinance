#ifndef MAINWINDOW_H
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

private:
    void createActions();
    void createMenus();


};
#endif // MAINWINDOW_H

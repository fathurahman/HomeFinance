#ifndef TRANSACTIONTABLEVIEW_H
#define TRANSACTIONTABLEVIEW_H

#include <QTableView>
#include <QAction>

class TransactionTableView : public QTableView
{
    Q_OBJECT
public:
    explicit TransactionTableView(QWidget *parent = nullptr);

signals:

private:
    QModelIndex m_selectedIndex;
    QAction* m_editSelectedAction;
    QAction* m_deleteSelectedAction;

private slots:
    void onCustomContextMenuRequested(QPoint);
    void editSelected();
    void deleteSelected();
};

#endif // TRANSACTIONTABLEVIEW_H

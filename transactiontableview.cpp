#include "transactiontableview.h"
#include "application.h"
#include <QMenu>
#include <QStyleFactory>
#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>
#include "edittransactiondialog.h"

TransactionTableView::TransactionTableView(QWidget *parent)
    : QTableView{parent}
{
    setModel(app->transactionTableModel());

    for (int i = 1; i <= 5; ++i)
    {
        setColumnWidth(i, 130);
    }
    setColumnWidth(2, 200);

    auto fusion = QStyleFactory::create("fusion");
    horizontalHeader()->setStyle(fusion);
    verticalHeader()->setStyle(fusion);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QTableView::customContextMenuRequested, this, &TransactionTableView::onCustomContextMenuRequested);

    m_editSelectedAction = new QAction("Edit", this);
    connect(m_editSelectedAction, &QAction::triggered, this, &TransactionTableView::editSelected);

    m_deleteSelectedAction = new QAction("Delete", this);
    connect(m_deleteSelectedAction, &QAction::triggered, this, &TransactionTableView::deleteSelected);

}

void TransactionTableView::onCustomContextMenuRequested(QPoint pos)
{
    m_selectedIndex = indexAt(pos);
    QMenu *menu = new QMenu(this);
    menu->addAction(m_editSelectedAction);
    menu->addSeparator();
    menu->addAction(m_deleteSelectedAction);
    menu->popup(viewport()->mapToGlobal(pos));
}

void TransactionTableView::editSelected()
{
    const int index = m_selectedIndex.row();
    EditTransactionDialog d(index, this);
    int ret = d.exec();
    if (ret)
    {
        db->editTransaction(index, d.transaction());
    }
}

void TransactionTableView::deleteSelected()
{
    const int index = m_selectedIndex.row();
    const Transaction tx = db->transaction(index);
    const QString text = QString("Delete trx on %1?").arg(tx.date.toString());
    const QString info = QString("@%1\n%2\nRp. %L3\n(%4)").arg(tx.locationName()).arg(tx.itemName()).arg(tx.value).arg(tx.walletName());

    QMessageBox box;
    box.setText(text);
    box.setInformativeText(info);
    box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    box.setDefaultButton(QMessageBox::Cancel);

    const int ret = box.exec();
    if (ret == QMessageBox::Ok)
    {
        db->deleteTransaction(index);
    }
}

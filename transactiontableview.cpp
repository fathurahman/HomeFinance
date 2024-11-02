#include "transactiontableview.h"
#include "application.h"
#include <QMenu>
#include <QStyleFactory>
#include <QHeaderView>
#include <QMenu>

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

}

void TransactionTableView::deleteSelected()
{

}

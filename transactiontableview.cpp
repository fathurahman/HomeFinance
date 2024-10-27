#include "transactiontableview.h"
#include "application.h"
#include <QMenu>
#include <QStyleFactory>
#include <QHeaderView>

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
}

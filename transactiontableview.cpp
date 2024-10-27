#include "transactiontableview.h"
#include "application.h"
#include <QMenu>
#include <QStyleFactory>
#include <QHeaderView>

TransactionTableView::TransactionTableView(QWidget *parent)
    : QTableView{parent}
{
    setModel(app->transactionTableModel());

    setColumnWidth(0, 260);
    setColumnWidth(1, 220);
    for (int i = 2; i < 4; ++i)
    {
        setColumnWidth(i, 130);
    }

    auto fusion = QStyleFactory::create("fusion");
    horizontalHeader()->setStyle(fusion);
    verticalHeader()->setStyle(fusion);
}

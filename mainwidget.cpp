#include "mainwidget.h"
#include "application.h"
#include "filterwidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTableView>
#include <QStyleFactory>
#include <QHeaderView>

MainWidget::MainWidget(QWidget* parent)
	: QWidget(parent)
{
    auto* tableView = new QTableView();
    tableView->setModel(app->transactionTableModel());
    tableView->setColumnWidth(0, 260);
    tableView->setColumnWidth(1, 220);
    for (int i = 2; i < 4; ++i)
        tableView->setColumnWidth(i, 130);
    tableView->setColumnWidth(4, 130);
    auto fusion = QStyleFactory::create("fusion");
    tableView->horizontalHeader()->setStyle(fusion);
    tableView->verticalHeader()->setStyle(fusion);


    auto* filterWidget = new FilterWidget();

    auto* layout = new QVBoxLayout;
    layout->addWidget(filterWidget);
    layout->addWidget(tableView, 1);
    setLayout(layout);
}

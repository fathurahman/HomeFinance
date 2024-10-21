#include "mainwidget.h"
#include "application.h"
#include "filterwidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTableView>

MainWidget::MainWidget(QWidget* parent)
	: QWidget(parent)
{
    auto* tableView = new QTableView();
    tableView->setModel(app->transactionTableModel());
    tableView->setColumnWidth(0, 160);
    tableView->setColumnWidth(1, 160);
    for (int i = 2; i < 4; ++i)
        tableView->setColumnWidth(i, 130);
    tableView->setColumnWidth(4, 140);

    auto* filterWidget = new FilterWidget();

    auto* layout = new QVBoxLayout;
    layout->addWidget(filterWidget);
    layout->addWidget(tableView, 1);
    setLayout(layout);
}

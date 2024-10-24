#include "filterwidget.h"
#include "application.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

FilterWidget::FilterWidget(QWidget* parent)
	: QWidget(parent)
{    
    ui_year = new QSpinBox;
    ui_year->setRange(-1, 2080);
    ui_year->setValue(-1);

    ui_month = new QSpinBox;
    ui_month->setRange(-1, 12);
    ui_month->setValue(-1);

    ui_day = new QSpinBox;
    ui_day->setRange(-1, 31);
    ui_day->setValue(-1);

    ui_wallet = new QComboBox;
    ui_wallet->setMinimumWidth(160);
    updateWallet();
    connect(db, &Database::walletAdded, this, &FilterWidget::updateWallet);

    ui_flow = new QComboBox;
    ui_flow->addItems({"Any", "Debit", "Credit"});
    ui_flow->setCurrentIndex(0);

    ui_keyword = new QLineEdit;

    auto* resetButton = new QPushButton("Reset");
    connect(resetButton, &QPushButton::clicked, this, &FilterWidget::resetFilter);

    auto* applyButton = new QPushButton("Apply");
    connect(applyButton, &QPushButton::clicked, this, &FilterWidget::applyFilter);

    auto* l = new QHBoxLayout;
    l->addWidget(resetButton);
    l->addWidget(new QLabel("Year:"));
    l->addWidget(ui_year);
    l->addWidget(new QLabel("Month:"));
    l->addWidget(ui_month);
    l->addWidget(new QLabel("Day:"));
    l->addWidget(ui_day);
    l->addWidget(new QLabel("Wallet:"));
    l->addWidget(ui_wallet);
    l->addWidget(new QLabel("Flow:"));
    l->addWidget(ui_flow);
    l->addWidget(new QLabel("Keyword:"));
    l->addWidget(ui_keyword);
    l->addWidget(applyButton);
    setLayout(l);
}

void FilterWidget::updateWallet()
{
    QString cur = ui_wallet->currentText();
    ui_wallet->clear();
    auto names = db->walletNames();
    names.push_front("Any");
    ui_wallet->addItems(names);
    if (cur.isEmpty())
    {
        ui_wallet->setCurrentIndex(0);
    }
    else
    {
        ui_wallet->setCurrentText(cur);
    }

}

void FilterWidget::resetFilter()
{
    ui_year->setValue(-1);
    ui_month->setValue(-1);
    ui_day->setValue(-1);
    ui_wallet->setCurrentIndex(0);
    ui_flow->setCurrentIndex(0);
    ui_keyword->setText("");
    app->transactionTableModel()->setFilter(TransactionFilter());
}

void FilterWidget::applyFilter()
{
    TransactionFilter f;
    f.year = ui_year->value();
    f.month = ui_month->value();
    f.day = ui_day->value();
    f.walletIndex = ui_wallet->currentIndex() - 1;
    f.flow = ui_flow->currentIndex();
    auto key = ui_keyword->text();
    f.itemName = key;
    f.tagName = key;
    f.locationName = key;
    app->transactionTableModel()->setFilter(f);
}

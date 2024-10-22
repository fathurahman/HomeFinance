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

    ui_locationName = new QLineEdit;
    ui_itemName = new QLineEdit;
    ui_tagName = new QLineEdit;

    ui_wallet = new QComboBox;
    ui_wallet->setMinimumWidth(160);
    updateWallet();
    connect(db, &Database::walletAdded, this, &FilterWidget::updateWallet);

    ui_flow = new QComboBox;
    ui_flow->addItems({"Any", "Debit", "Credit"});
    ui_flow->setCurrentIndex(0);

    auto* reset = new QPushButton("Reset");
    connect(reset, &QPushButton::clicked, this, &FilterWidget::resetFilter);

    auto* apply = new QPushButton("Apply");
    connect(apply, &QPushButton::clicked, this, &FilterWidget::applyFilter);

    auto* top = new QHBoxLayout;
    top->addWidget(new QLabel("Year:"));
    top->addWidget(ui_year);
    top->addWidget(new QLabel("Month:"));
    top->addWidget(ui_month);
    top->addWidget(new QLabel("Day:"));
    top->addWidget(ui_day);
    top->addWidget(new QLabel("Wallet:"));
    top->addWidget(ui_wallet);
    top->addWidget(new QLabel("Flow:"));
    top->addWidget(ui_flow);

    auto* bot = new QHBoxLayout;
    bot->addWidget(new QLabel("Location:"));
    bot->addWidget(ui_locationName);
    bot->addWidget(new QLabel("Item:"));
    bot->addWidget(ui_itemName);
    bot->addWidget(new QLabel("Tag:"));
    bot->addWidget(ui_tagName);

    auto* fbox = new QVBoxLayout;
    fbox->addLayout(top);
    fbox->addLayout(bot);

    auto* bbox = new QVBoxLayout;
    bbox->addWidget(reset);
    bbox->addWidget(apply);

    auto* layout = new QHBoxLayout;
    layout->addLayout(fbox);
    layout->addLayout(bbox);
    setLayout(layout);
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
    ui_locationName->setText("");
    ui_itemName->setText("");
    ui_tagName->setText("");
    ui_wallet->setCurrentIndex(0);
    ui_flow->setCurrentIndex(0);
    app->transactionTableModel()->setFilter(TransactionFilter());
}

void FilterWidget::applyFilter()
{
    TransactionFilter f;
    f.year = ui_year->value();
    f.month = ui_month->value();
    f.day = ui_day->value();
    f.locationName = ui_locationName->text();
    f.itemName = ui_itemName->text();
    f.tagName = ui_tagName->text();
    f.walletIndex = ui_wallet->currentIndex() - 1;
    f.flow = ui_flow->currentIndex();
    app->transactionTableModel()->setFilter(f);
}

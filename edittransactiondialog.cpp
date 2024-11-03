#include "edittransactiondialog.h"
#include "application.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

EditTransactionDialog::EditTransactionDialog(int index, QWidget *parent)
    : QDialog{parent}
    , m_index(index)
{
    setWindowTitle(QString("Edit Transaction %1").arg(index));

    const auto& tx = db->transaction(index);

    ui_date = new QDateEdit;
    ui_date->setDate(tx.date);

    ui_locationName = new QLineEdit;
    ui_locationName->setText(tx.locationName());
    ui_locationName->setCompleter(db->createLocationNameCompleter(this));

    ui_type = new QComboBox;
    ui_type->addItem("Debit");
    ui_type->addItem("Credit");
    ui_type->setCurrentIndex((tx.value >= 0) ? 0 : 1);

    ui_itemName = new QLineEdit;
    ui_itemName->setText(db->itemNames()[tx.itemIndex]);
    ui_itemName->setCompleter(db->createItemNameCompleter(this));

    ui_quantity = new QSpinBox;
    ui_quantity->setRange(1, 999);
    ui_quantity->setValue(tx.num);

    ui_value = app->createNumLineEdit((tx.value < 0) ? -tx.value : tx.value);

    ui_walletName = new QComboBox;
    const auto walletNames = db->walletNames();
    for (const auto& walletName : walletNames)
    {
        ui_walletName->addItem(walletName);
    }
    ui_walletName->setCurrentIndex(tx.walletIndex);

    auto* form = new QFormLayout;
    form->addRow("Date:", ui_date);
    form->addRow("Location:", ui_locationName);
    form->addRow("Type:", ui_type);
    form->addRow("Item:", ui_itemName);
    form->addRow("Quantity:", ui_quantity);
    form->addRow("Value:", ui_value);
    form->addRow("Wallet:", ui_walletName);

    auto* applyButton = new QPushButton("Apply");
    connect(applyButton, &QPushButton::clicked, this, &QDialog::accept);

    auto* cancelButton = new QPushButton("Cancel");
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    auto* buttonsBox = new QHBoxLayout;
    buttonsBox->addStretch(1);
    buttonsBox->addWidget(applyButton);
    buttonsBox->addWidget(cancelButton);

    auto* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(form);
    mainLayout->addLayout(buttonsBox);

    setLayout(mainLayout);
}

Transaction EditTransactionDialog::transaction() const
{
    Transaction tx;
    tx.date = ui_date->date();
    tx.locationIndex = db->getOrAddLocationIndexByName(ui_locationName->text());
    tx.walletIndex = ui_walletName->currentIndex();
    tx.itemIndex = db->getOrAddItemIndexByName(ui_itemName->text());
    tx.num = ui_quantity->value();
    auto value = ui_value->text().toLongLong();
    value = (ui_type->currentIndex() == 0) ? value : -value;
    tx.value = value;
    return tx;
}

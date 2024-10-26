#include "addjournaldialog.h"
#include <QScrollArea>
#include <QFormLayout>
#include <QPushButton>
#include <QCompleter>
#include <QLabel>
#include "application.h"
#include "database.h"

AddJournalDialog::AddJournalDialog(bool isDebit, QWidget *parent)
    : QDialog{parent}
{
    setWindowTitle("Add Journal");
    setMinimumSize(480, 600);

    m_itemNameCompleter = db->createItemNameCompleter(this);

    ui_type = new QComboBox();
    ui_type->addItem("Debit");
    ui_type->addItem("Credit");
    ui_type->setCurrentIndex(isDebit ? 0 : 1);

    ui_date = new QDateEdit();
    ui_date->setDate(QDate::currentDate());

    ui_location = new QLineEdit();
    ui_location->setCompleter(db->createLocationNameCompleter(this));

    ui_wallet = new QComboBox();
    if (db->walletsNum() == 0)
    {
        ui_wallet->setCurrentText("New Wallet");
    }
    else
    {
        ui_wallet->addItems(db->walletNames());
        ui_wallet->setCurrentIndex(db->activeWalletIndex());
    }
    ui_wallet->setEditable(true);

    auto* form = new QFormLayout;
    form->addRow("Type:", ui_type);
    form->addRow("Date:", ui_date);
    form->addRow("Location:", ui_location);
    form->addRow("Wallet:", ui_wallet);

    ui_rowBox = new QVBoxLayout;
    addRow();

    auto addEntry = new QPushButton("Add Entry");
    connect(addEntry, &QPushButton::clicked, this, &AddJournalDialog::addRow);

    QVBoxLayout* rows = new QVBoxLayout;
    rows->addLayout(ui_rowBox);
    rows->addWidget(addEntry);
    rows->addStretch(1);

    auto* add = new QPushButton("Add Journal");
    connect(add, &QPushButton::clicked, this, &QDialog::accept);

    auto* cancel = new QPushButton("Cancel");
    connect(cancel, &QPushButton::clicked, this, &QDialog::reject);

    auto* buttons = new QHBoxLayout;
    buttons->addWidget(add);
    buttons->addWidget(cancel);

    auto* layout = new QVBoxLayout;
    layout->addLayout(form);
    layout->addWidget(new QLabel("Entries:"));
    layout->addLayout(rows, 1);
    layout->addLayout(buttons);
    setLayout(layout);

    ui_location->setFocus();
}


JournalForm AddJournalDialog::journalForm() const
{
    JournalForm j;
    j.date = ui_date->date();
    j.locationName = ui_location->text();
    j.walletName = ui_wallet->currentText();
    j.isDebit = ui_type->currentIndex() == 0;
    for (const auto& row : ui_rows)
    {
        JournalEntryForm e;
        e.itemName = row.name->text();
        e.num = row.quantity->value();
        e.value = row.value->text().toLongLong();
        if (e.value != 0.0)
        {
            j.entryForms.append(e);
        }
    }
    return j;
}


void AddJournalDialog::addRow()
{
    JournalRow row;

    row.name = new QLineEdit;
    row.name->setCompleter(m_itemNameCompleter);

    row.quantity = new QSpinBox;
    row.quantity->setRange(1, 9990);

    row.value = app->createNumLineEdit();

    ui_rows.append(row);

    auto layout = new QHBoxLayout;
    layout->addWidget(row.name, 2);
    layout->addWidget(row.quantity);
    layout->addWidget(row.value, 2);        

    ui_rowBox->addLayout(layout);

    if (ui_rows.size() > 1)
    {
        row.name->setFocus();
    }
}

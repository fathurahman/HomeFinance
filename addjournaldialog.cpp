﻿#include "addjournaldialog.h"
#include <QScrollArea>
#include <QFormLayout>
#include <QPushButton>
#include <QCompleter>
#include "application.h"
#include "database.h"

AddJournalDialog::AddJournalDialog(bool isDebit, QWidget *parent)
    : QDialog{parent}
{
    setWindowTitle(QString("Add %1 Journal").arg(isDebit ? "Debit" : "Credit"));
    setMinimumSize(480, 800);

    m_itemNameCompleter = db->createItemNameCompleter(this);

    ui_dateTime = new QDateTimeEdit(QDateTime::currentDateTime());

    ui_wallet = new QComboBox();
    if (db->hasAnyWallet() == false) {
        ui_wallet->setCurrentText("New Wallet");
    }
    else {
        const auto names = db->walletNames();
        for (const auto& name : names) {
            ui_wallet->addItem(name);
        }
        ui_wallet->setCurrentIndex(db->activeWallet());
    }
    ui_wallet->setEditable(true);

    ui_type = new QComboBox();
    ui_type->addItem("Debit");
    ui_type->addItem("Credit");
    ui_type->setCurrentIndex(isDebit ? 0 : 1);

    ui_location = new QLineEdit();
    ui_location->setCompleter(db->createLocationNameCompleter(this));

    auto* form = new QFormLayout;
    form->addRow("DateTime:", ui_dateTime);
    form->addRow("Type:", ui_type);
    form->addRow("Wallet:", ui_wallet);
    form->addRow("Location:", ui_location);


    ui_rowBox = new QVBoxLayout;
    for (int i = 0; i < 30; ++i)
    {
        addRow();
    }
    QWidget* rows = new QWidget;
    rows->setMinimumWidth(440);
    rows->setLayout(ui_rowBox);

    auto* scrollArea = new QScrollArea;
    scrollArea->setWidget(rows);

    auto* ok = new QPushButton("Add");
    connect(ok, &QPushButton::clicked, this, &QDialog::accept);
    auto* cancel = new QPushButton("Cancel");
    connect(cancel, &QPushButton::clicked, this, &QDialog::reject);
    auto* buttons = new QHBoxLayout;
    buttons->addWidget(ok);
    buttons->addWidget(cancel);

    auto* layout = new QVBoxLayout;
    layout->addLayout(form);
    layout->addWidget(scrollArea, 1);
    layout->addLayout(buttons);
    setLayout(layout);

    ui_wallet->setFocus();
}


JournalData AddJournalDialog::journalData() const
{
    JournalData j;
    j.dateTime  = ui_dateTime->dateTime();
    j.location  = db->getOrAddLocation(ui_location->text());
    j.wallet    = db->getOrAddWallet(ui_wallet->currentText());
    j.isDebit   = (ui_type->currentIndex() == 0);
    const int n = ui_rows.size();
    for (int i = 0; i < n; ++i) {
        const auto& row = ui_rows[i];
        QString name    = row.name->text();
        int num         = row.quantity->value();
        qint64 value    = row.value->text().toLongLong();
        if (!name.isEmpty() && num > 0 && value > 0) {
            JournalEntryData e;
            e.item  = db->getOrAddItem(name);
            e.num   = num;
            e.value = value;
            j.entryDataList.append(e);
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
}

﻿#include "addwalletdialog.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSpacerItem>
#include "application.h"
#include "database.h"

AddWalletDialog::AddWalletDialog(QWidget *parent)
    : QDialog{parent}
{
    setWindowTitle("Add Wallet");
    setMinimumWidth(240);

    ui_name = new QLineEdit();
    connect(ui_name, &QLineEdit::textEdited, this, &AddWalletDialog::onNameEdited);

    ui_value = app->createNumLineEdit();

    ui_isCredit = new QCheckBox();
    ui_isCredit->setChecked(false);


    auto* form = new QFormLayout;
    form->addRow("Name:", ui_name);
    form->addRow("Value:", ui_value);
    form->addRow("Credit Account:", ui_isCredit);

    auto* add = new QPushButton("Add");
    connect(add, &QPushButton::clicked, this, &QDialog::accept);
    auto* cancel = new QPushButton("Cancel");
    connect(cancel, &QPushButton::clicked, this, &QDialog::reject);

    ui_addButton = add;
    ui_addButton->setEnabled(false);

    auto* buttons = new QHBoxLayout;
    buttons->addWidget(add, 1);
    buttons->addItem(new QSpacerItem(20, 0));
    buttons->addWidget(cancel, 1);

    auto* layout = new QVBoxLayout;
    layout->addLayout(form);
    layout->addLayout(buttons);
    setLayout(layout);

    ui_name->setFocus();
}

Wallet AddWalletDialog::wallet() const
{
    return Wallet(
                ui_name->text(),
                ui_value->text().toLongLong(),
                ui_isCredit->isChecked());
}

void AddWalletDialog::onNameEdited(const QString &text)
{
    if (text.isEmpty() || db->wallet(text) != nullptr)
	{
        ui_addButton->setEnabled(false);
        ui_name->setStyleSheet("background-color: red");
    }
    else
	{
        ui_addButton->setEnabled(true);
        ui_name->setStyleSheet("background-color: white");
    }
}

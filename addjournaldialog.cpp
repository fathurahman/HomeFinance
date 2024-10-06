#include "addjournaldialog.h"
#include <QScrollArea>
#include <QFormLayout>
#include <QPushButton>
#include "application.h"
#include "database.h"

AddJournalDialog::AddJournalDialog(bool isDebit, QWidget *parent)
    : QDialog{parent}
    , m_isDebit(isDebit)
{
    setWindowTitle(QString("Add %1 Journal").arg(isDebit ? "Debit" : "Credit"));
    setMinimumSize(480, 800);

    ui_dateTime = new QDateTimeEdit(QDateTime::currentDateTime());
    ui_wallet = new QComboBox();
    ui_wallet->setEditable(true);
    ui_wallet->setEditText("Ucup");
    ui_title = new QLineEdit;
    auto* form = new QFormLayout;
    form->addRow("DateTime:", ui_dateTime);
    form->addRow("Wallet:", ui_wallet);
    form->addRow("Title:", ui_title);


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

    ui_title->setFocus();
}

Journal AddJournalDialog::journal() const
{
    Journal j;
    j.dateTime = ui_dateTime->dateTime();
    return j;
}



void AddJournalDialog::addRow()
{
    JournalRow row;
    row.name = new QLineEdit;
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

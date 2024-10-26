#include "viewwalletsdialog.h"
#include "database.h"
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

ViewWalletsDialog::ViewWalletsDialog(QWidget *parent)
    : QDialog{parent}
{
    setWindowTitle("Wallets");

    auto* closeButton = new QPushButton("Close");
    connect(closeButton, &QPushButton::clicked, this, &ViewWalletsDialog::accept);

    auto* l = new QVBoxLayout;
    l->addLayout(makeRows(), 1);
    l->addWidget(closeButton);
    setLayout(l);
}

QHBoxLayout* ViewWalletsDialog::makeRow(const QString &name, qint64 value)
{
    QLabel *l, *r;
    l = new QLabel(QString("%1 :").arg(name));
    l->setAlignment(Qt::AlignRight);
    r = new QLabel(QString("Rp. %L1").arg(value));
    r->setAlignment(Qt::AlignRight);

    auto* row = new QHBoxLayout;
    row->addWidget(l, 1);
    row->addWidget(r, 1);

    return row;
}

QVBoxLayout* ViewWalletsDialog::makeRows()
{
    auto* rows = new QVBoxLayout;

    const int n = db->walletsNum();
    for (int i = 0; i < n; ++i)
    {
        const auto* wallet = db->wallet(i);
        QString name;
        if (wallet->external)
        {
            name = QString("%1*").arg(wallet->name);
        }
        else
        {
            name = wallet->name;
        }
        auto* row = makeRow(name, wallet->value);
        rows->addLayout(row);
    }

    auto* hline = new QFrame();
    hline->setFrameStyle(QFrame::HLine | QFrame::Plain);
    rows->addWidget(hline);

    auto* row = makeRow("Total", db->totalValue());
    rows->addLayout(row);

    return rows;
}

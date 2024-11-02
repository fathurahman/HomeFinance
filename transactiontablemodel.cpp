#include "transactiontablemodel.h"
#include <QBrush>

TransactionTableModel::TransactionTableModel(QObject *parent)
    : QAbstractTableModel{parent}
{
    populatePointers();
    connect(db, &Database::transactionsModified, this, &TransactionTableModel::populatePointers);
}

void TransactionTableModel::setFilter(const TransactionFilter &filter)
{
    m_filter = filter;
    populatePointers();
}

int TransactionTableModel::rowCount(const QModelIndex &) const
{
    return m_pointers.size();
}

int TransactionTableModel::columnCount(const QModelIndex &) const
{
    return 7;
}

QVariant TransactionTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation == Qt::Horizontal) {
        switch(section) {
        case 0:
            return "Date";
        case 1:
            return "Location";
        case 2:
            return "Item";
        case 3:
            return "Debit";
        case 4:
            return "Credit";
        case 5:
            return "Balance";
        case 6:
            return "Wallet";
        default:
            return QVariant();
        }
    }
    return section + 1;
}

QVariant TransactionTableModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();
    const auto& p = m_pointers[row];
    const auto t = db->transaction(p);
    const int col = index.column();
    if (role == Qt::DisplayRole)
    {
        switch(col)
        {
        case 0:
            return t.date;
        case 1:
            return t.locationName();
        case 2:
            return t.itemName();
        case 3:
            return (t.value > 0.0) ? QString("Rp. %L1").arg(t.debit()) : QVariant();
        case 4:
            return (t.value < 0.0) ? QString("Rp. %L1").arg(t.credit()) : QVariant();
        case 5:
            return QString("Rp. %L1").arg(t.balance);
        case 6:
            return t.walletName();
        default:
            return QVariant();
        }
    }
    else if (role == Qt::TextAlignmentRole)
    {
        if (col == 3|| col == 4 || col == 5)
        {
            const int a = Qt::AlignRight | Qt::AlignVCenter;
            return a;
        }
        else if (col == 6)
        {
            return { Qt::AlignCenter };
        }
    }
    else if (role == Qt::BackgroundRole)
    {
        if (row % 2)
        {
            int c = 0xF0;
            return QBrush(QColor(c, c, c));
        }
    }
    return QVariant();

}

void TransactionTableModel::populatePointers()
{
    beginResetModel();
    m_pointers = db->filterTransactions(m_filter);
    endResetModel();
}

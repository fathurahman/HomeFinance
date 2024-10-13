#include "transactiontablemodel.h"

TransactionTableModel::TransactionTableModel(QObject *parent)
    : QAbstractTableModel{parent}
{
    populatePointers();
    connect(db, &Database::journalAdded, this, &TransactionTableModel::populatePointers);
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
            return "DateTime";
        case 1:
            return "Location";
        case 2:
            return "Wallet";
        case 3:
            return "Item";
        case 4:
            return "Debit";
        case 5:
            return "Credit";
        case 6:
            return "Balance";
        default:
            return QVariant();
        }
    }
    return section + 1;
}

QVariant TransactionTableModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    const int row = index.row();
    const auto& p = m_pointers[row];
    const auto t = db->transaction(p);

    const int col = index.column();
    switch(col) {
    case 0:
        return t.dateTime;
    case 1:
        return db->locationName(t.location);
    case 2:
        return db->walletName(t.wallet);
    case 3:
        return db->itemName(t.item, t.num);
    case 4:
        return t.debit;
    case 5:
        return t.credit;
    case 6:
        return t.balance;
    default:
        return QVariant();
    }
}

void TransactionTableModel::populatePointers()
{
    beginResetModel();
    m_pointers = db->filterTransactions(m_filter);
    endResetModel();
}

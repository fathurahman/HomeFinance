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
    return 6;
}

QVariant TransactionTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation == Qt::Horizontal) {
        switch(section) {
        case 0:
            return "Time & Place";
        case 1:
            return "Item";
        case 2:
            return "Debit";
        case 3:
            return "Credit";
        case 4:
            return "Balance";
        case 5:
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
            if (t.locationIndex < 0) return t.date;
            return QString("%1, %2").arg(t.date.toString("dd/MM/yyyy")).arg(db->locationName(t.locationIndex));
        case 1:
            return db->itemName(t.itemIndex, t.num);
        case 2:
            if (t.debit == 0) return QVariant();
            return QString("Rp. %L1").arg(t.debit);
        case 3:
            if (t.credit == 0) return QVariant();
            return QString("Rp. %L1").arg(t.credit);
        case 4:
            return QString("Rp. %L1").arg(t.balance);
        case 5:
            return db->wallet(t.walletIndex)->name;
        default:
            return QVariant();
        }
    }
    else
    {
        return QVariant();
    }

}

void TransactionTableModel::populatePointers()
{
    beginResetModel();
    m_pointers = db->filterTransactions(m_filter);
    endResetModel();
}

#ifndef TRANSACTIONTABLEMODEL_H
#define TRANSACTIONTABLEMODEL_H

#include <QAbstractTableModel>
#include "database.h"

class TransactionTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TransactionTableModel(QObject *parent = nullptr);

    inline const TransactionFilter& filter() const { return m_filter; }
    void setFilter(const TransactionFilter& filter);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role) const override;

public slots:
    void populatePointers();

private:
    TransactionFilter m_filter;
    QList<int> m_pointers;
};

#endif // TRANSACTIONTABLEMODEL_H

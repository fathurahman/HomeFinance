#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QStringList>
#include <QDateTime>
#include <QCompleter>
#include "datatypes.h"

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = nullptr);

    int version() const;

    inline bool isModified() const { return m_isModified; }
    inline QString lastFilePath() const { return m_lastFilePath; }
    bool load(const QString& path);
    bool save(const QString& path) const;

    int walletsNum() const;
    QStringList walletNames() const;
    const Wallet* wallet(int index) const;
    const Wallet *wallet(const QString& name) const;
    bool addWallet(const Wallet& wallet);
    inline int activeWalletIndex() const { return m_activeWalletIndex; }

    inline qint64 totalValue() const { return m_totalValue; }

    inline const QStringList& itemNames() const { return m_itemNames; }
    QCompleter* createItemNameCompleter(QObject* parent) const;

    inline const QStringList& locationNames() const { return m_locationNames; }    
    QCompleter* createLocationNameCompleter(QObject* parent);   
    inline int activeLocationIndex() const { return m_activeLocationIndex; }

    bool addJournal(const Journal& journal);

    inline const QList<Transaction> transactions() const { return m_transactions; }
    inline const Transaction& transaction(int index) const { return m_transactions[index]; }
    QList<int> filterTransactions(const TransactionFilter& filter) const;

    void deleteTransaction(int index);
    void editTransaction(int index, const Transaction& tx);

    int getOrAddWalletIndexByName(const QString& name);
    int getOrAddLocationIndexByName(const QString& name);
    int getOrAddItemIndexByName(const QString& name);

signals:
    void walletAdded();
    void transactionsModified();
    void totalValueChanged();

public slots:
    void sortTransactions();

private:
    QList<Wallet> m_wallets;
    QStringList m_itemNames;
    QStringList m_locationNames;
    QList<Transaction> m_transactions;

    mutable bool m_isModified = false;
    mutable QString m_lastFilePath;
    mutable int m_activeWalletIndex = 0;
    mutable int m_activeLocationIndex = 0;
    qint64 m_totalValue = 0;

private:
    void updateTotalValue(bool forceUpdate = false);    
    void updateTransactionBalancesForWallet(int walletIndex);
    void updateTransactionBalances();

    bool filterDate(const QDate &date, const TransactionFilter& filter) const;
    bool filterLocationIndex(int index, const TransactionFilter& filter) const;
    bool filterItemIndex(int index, const TransactionFilter& filter) const;
    bool filterKeyword(const Transaction& transaction, const TransactionFilter& filter) const;
    bool filterTransaction(const Transaction& transaction, const TransactionFilter& filter) const;
};

extern Database *db;


#endif // DATABASE_H

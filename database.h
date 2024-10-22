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

    inline QList<TaggedName>& items() { return m_items; }

    QString itemName(int index, int num = 0) const;

    QCompleter* createItemNameCompleter(QObject* parent) const;

    inline QList<TaggedName>& locations() { return m_locations; }

    QString locationName(int locationIndex) const;

    QCompleter* createLocationNameCompleter(QObject* parent);   

    QCompleter* createTagNameCompleter(QObject* parent);

    bool addJournal(const JournalForm& form);

    QList<TransactionPointer> filterTransactions(const TransactionFilter& filter) const;

    Transaction transaction(const TransactionPointer& ptr) const;

signals:
    void loading();
    void loaded();
    void totalValueChanged();
    void walletAdded();
    void journalAdded();

private:
    QList<Wallet> m_wallets;
    QList<TaggedName> m_items;
    QList<TaggedName> m_locations;
    QList<Journal> m_journals;
    QStringList m_tagNames;

    mutable bool m_isModified = false;
    mutable QString m_lastFilePath;
    int m_activeWalletIndex = 0;
    qint64 m_totalValue = 0;

private:
    int getOrAddWalletIndexByName(const QString& name);
    int getOrAddLocationIndexByName(const QString& name);
    int getOrAddItemIndexByName(const QString& name);


    TransactionFilter normalizeFilter(const TransactionFilter& filter) const;
    bool filterDate(const QDate &date, const TransactionFilter& filter) const;
    bool filterLocationIndex(int index, const TransactionFilter& filter) const;
    bool filterItemIndex(int index, const TransactionFilter& filter) const;
    bool filterJournal(const Journal& journal, const TransactionFilter& filter) const;
    bool filterJournalEntry(const JournalEntry& entry, const TransactionFilter& filter) const;
};

extern Database *db;


#endif // DATABASE_H

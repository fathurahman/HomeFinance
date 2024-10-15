#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QStringList>
#include <QDateTime>
#include <QCompleter>

struct WalletData
{
    QString name;
    qint64 value;
};

struct JournalEntryData
{
    int item;
    int num;
    qint64 value;
    qint64 balance;
};

struct JournalData
{
    QDateTime dateTime;
    int location;
    int wallet;
    bool isDebit;
    QList<JournalEntryData> entryDataList;
};

struct Transaction
{
    QDateTime dateTime;
    int location;
    int wallet;
    int item;
    int num;
    qint64 debit;
    qint64 credit;
    qint64 balance;
};

struct TransactionFilter
{
    int year = -1;
    int month = -1;
    int day = -1;
    QString location;
    int wallet = -1;
    QString item;

    bool hasLocation;
    int specificLocation;

    bool hasItem;
    int specificItem;
};

struct TransactionPointer
{
    int journal;
    int entry;
};

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = nullptr);

    inline bool isModified() const { return m_isModified; }

    // The last file path from/to which the database was loaded/saved
    inline QString lastFilePath() const { return m_lastFilePath; }

    // Check if there's any wallet at all in the database
    inline bool hasAnyWallet() const { return m_walletDataList.size() > 0; }

    // Check whether there's a wallet with the specified name in the database
    bool hasWalletWithName(const QString& name) const;

    QStringList walletNames() const;

    QString walletName(int wallet) const;

    int addWallet(const WalletData& data);

    int getOrAddWallet(const QString& name);

    inline int activeWallet() const { return m_activeWallet; }

    inline QString locationName(int index) const {
        if (index < 0 || index >= m_locationNames.size())
            return QString();
        return m_locationNames.at(index);
    }
    inline QString itemName(int index, int num = 1) const {
        if (index < 0 || index >= m_itemNames.size())
            return QString();
        if (num <= 1) {
            return m_itemNames.at(index);
        } else {
            return QString("%1 x%2").arg(m_itemNames.at(index)).arg(num);
        }
    }

    QCompleter* createItemNameCompleter(QObject* parent);
    QCompleter* createLocationNameCompleter(QObject* parent);

    int getOrAddLocation(const QString& name);
    int getOrAddItem(const QString& name);

    int addJournal(const JournalData& data);

    QList<TransactionPointer> filterTransactions(const TransactionFilter& filter) const;
    Transaction transaction(const TransactionPointer& ptr) const;

    inline qint64 totalValue() const { return m_totalValue; }

    bool load(const QString& path);
    bool save(const QString& path) const;

signals:
    void loading();
    void loaded();
    void totalValueChanged();
    void journalAdded();

private:
    mutable bool m_isModified = false;
    mutable QString m_lastFilePath;

    QList<WalletData> m_walletDataList;
    QList<JournalData> m_journalDataList;
    QStringList m_itemNames;
    QStringList m_locationNames;

    int m_activeWallet = 0;

    qint64 m_totalValue = 0;

private:
    TransactionFilter normalizeFilter(const TransactionFilter& filter) const;
    bool filterDateTime(const QDateTime &dateTime, const TransactionFilter& filter) const;
    bool filterLocation(int location, const TransactionFilter& filter) const;
    bool filterItem(int item, const TransactionFilter& filter) const;
    bool filterJournalData(const JournalData& data, const TransactionFilter& filter) const;
};

extern Database *db;


#endif // DATABASE_H

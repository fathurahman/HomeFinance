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
    QString location;
    int wallet = -1;
    QString item;
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

    bool hasWalletWithName(const QString& name) const;
    int addWallet(const WalletData& data);

    bool hasAnyWallet() const { return m_walletDataList.size() > 0; }
    QStringList walletNames() const;

    inline int activeWallet() const { return m_activeWallet; }

    inline const QString& walletName(int index) const {
        return m_walletDataList[index].name;
    }
    inline const QString& locationName(int index) const {
        return m_locationNames.at(index);
    }
    inline QString itemName(int index, int num = 1) const {
        if (num <= 1) {
            return m_itemNames.at(index);
        } else {
            return QString("%1 x%2").arg(m_itemNames.at(index)).arg(num);
        }
    }

    QCompleter* createItemNameCompleter(QObject* parent);
    QCompleter* createLocationNameCompleter(QObject* parent);

    int getOrAddWallet(const QString& name);
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
    QList<WalletData> m_walletDataList;
    QList<JournalData> m_journalDataList;
    QStringList m_itemNames;
    QStringList m_locationNames;

    int m_activeWallet = 0;

    qint64 m_totalValue = 0;
};

extern Database *db;


#endif // DATABASE_H

#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QStringList>
#include <QDateTime>

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
    qint64 credit;
    qint64 debit;
    qint64 balance;
};

class Database : public QObject
{
    Q_OBJECT
public:

    QList<WalletData> walletDataList;
    QList<JournalData> journalDataList;
    QStringList itemNames;
    QStringList locationNames;

    int activeWallet = -1;

public:
    explicit Database(QObject *parent = nullptr);

    int getOrAddWallet(const QString& name);

    int getOrAddLocation(const QString& name);

    int getOrAddItem(const QString& name);

    qint64 totalValue() const;        

    bool load(const QString& path);
    bool save(const QString& path) const;

signals:
    void loading();
    void loaded();
};

extern Database *db;


#endif // DATABASE_H

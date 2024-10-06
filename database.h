#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QStringList>
#include <QDateTime>

struct Wallet
{
    QString name;
    qint64 value;
};

struct JournalEntry
{
    int item;
    int num;
    qint64 value;
};

struct Journal
{
    QDateTime dateTime;
    int location;
    int wallet;
    bool isDebit;
    QList<JournalEntry> entries;
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

    QList<Wallet> wallets;
    QStringList itemNames;
    QStringList locationNames;
    QList<Journal> journals;

public:
    explicit Database(QObject *parent = nullptr);

    qint64 totalValue() const;

    bool load(const QString& path);
    bool save(const QString& path) const;

signals:
    void loading();
    void loaded();
};

extern Database *db;


#endif // DATABASE_H

#ifndef DATATYPES_H
#define DATATYPES_H

#include <QString>
#include <QDate>

struct Wallet
{
    QString name;
    qint64 value;
};

struct NameAndTags
{
    QString name;
    QList<int> tags;
};

struct Location
{
    QString name;
    QList<int> tags;
};

struct JournalEntry
{
    int itemIndex;
    int num;
    qint64 value;
    qint64 balance;
};

struct Journal
{
    QDate date;
    int locationIndex;
    int walletIndex;
    bool isDebit;
    QList<JournalEntry> entries;
};

struct Transaction
{
    QDate date;
    int locationIndex;
    int itemIndex;
    int num;
    int walletIndex;
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
    int journalIndex;
    int entryIndex;
};


#endif // DATATYPES_H

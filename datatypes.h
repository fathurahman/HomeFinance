#ifndef DATATYPES_H
#define DATATYPES_H

#include <QString>
#include <QDate>

struct Wallet
{
    QString name;
    qint64 value;

    Wallet() {}
    Wallet(const QString& inName, qint64 inValue = 0) : name(inName), value(inValue) {}
};

struct TaggedName
{
    QString name;
    QList<int> tagIndices;

    TaggedName() {}
    TaggedName(const QString& inName) : name(inName) {}
};


struct JournalEntry
{
    int itemIndex;
    int num;
    qint64 value;
    qint64 balance;
};

struct JournalEntryForm
{
    QString itemName;
    int num;
    qint64 value;
};

struct Journal
{
    QDate date;
    int locationIndex;
    int walletIndex;
    bool isDebit;
    QList<JournalEntry> entries;
};

struct JournalForm
{
    QDate date;
    QString locationName;
    QString walletName;
    bool isDebit;
    QList<JournalEntryForm> entryForms;
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

    QString locationName() const;
    QString itemName() const;
};

struct TransactionFilter
{
    int year = -1;
    int month = -1;
    int day = -1;
    QString locationName;
    QString itemName;
    QString tagName;
    int walletIndex = -1;
    int flow = 0;

    bool hasLocationName;
    int locationIndex;

    bool hasItemName;
    int itemIndex;

    int tagIndex;
};

struct TransactionPointer
{
    int journalIndex;
    int entryIndex;

    TransactionPointer() {}
    TransactionPointer(int ji, int ei) : journalIndex(ji), entryIndex(ei) {}
};


#endif // DATATYPES_H

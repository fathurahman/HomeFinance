#ifndef DATATYPES_H
#define DATATYPES_H

#include <QString>
#include <QDate>

#define FILE_EXT ".hf"

struct Wallet
{
    QString name;
    bool isCredit = false;
    qint64 initialValue = 0;
    qint64 value = 0;

    Wallet() {}
    Wallet(const QString& inName, qint64 inInitialValue = 0, bool inIsCredit = false)
        : name(inName)
        , isCredit(inIsCredit)
        , initialValue(inInitialValue)
        , value(inInitialValue)
    {
    }
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
    QString itemName;
    int num;
    qint64 value;
};

struct Journal
{
    QDate date;
    QString locationName;
    QString walletName;
    bool isDebit;
    QList<JournalEntry> entries;
};

struct Transaction
{
    QDate date;
    int locationIndex;
    int walletIndex;
    int itemIndex;
    int num;
    qint64 value;

    qint64 balance;

    QString locationName() const;
    QString walletName() const;
    QString itemName() const;
    qint64 debit() const;
    qint64 credit() const;

    inline bool operator==(const Transaction& o) const
    {
        return date == o.date
                && locationIndex == o.locationIndex
                && walletIndex == o.walletIndex
                && itemIndex == o.itemIndex
                && num == o.num
                && value == o.value;
    }

};




#define TXF_FLOW_NONE   0
#define TXF_FLOW_DEBIT  1
#define TXF_FLOW_CREDIT 2

struct TransactionFilter
{
    int year = -1;
    int month = -1;
    int day = -1;
    QString locationName;
    QString itemName;
    QString keyword;
    int walletIndex = -1;
    int flow = 0;

    bool isEmpty;

    bool hasKeyword;

    bool hasLocationName;
    int locationIndex;

    bool hasItemName;
    int itemIndex;

    TransactionFilter process() const;

};

struct TransactionPointer
{
    int journalIndex;
    int entryIndex;

    TransactionPointer() {}
    TransactionPointer(int ji, int ei) : journalIndex(ji), entryIndex(ei) {}
};


#endif // DATATYPES_H

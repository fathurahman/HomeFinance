#ifndef DATABASEFILE_H
#define DATABASEFILE_H

#include <QObject>
#include <QFile>
#include "datatypes.h"

class DatabaseFile : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseFile(QObject *parent = nullptr);

    bool open(const QString& path, QIODevice::OpenMode mode);

    bool readBool();
    void writeBool(bool value);

    int readInt();
    void writeInt(int value);

    qint64 readValue();
    void writeValue(qint64 value);

    QString readString();
    void writeString(const QString& str);

    QDate readDate();
    void writeDate(const QDate& date);

    void writeStringList(const QStringList& list);
    void readStringList(QStringList& list);

    void writeWallets(const QList<Wallet>& wallets);
    void readWallets(QList<Wallet>& wallets);

    Transaction readTransaction();
    void writeTransaction(const Transaction& tx);

    void readTransactions(QList<Transaction>& list);
    void writeTransactions(const QList<Transaction>& list);


private:
    QFile file;
};

#endif // DATABASEFILE_H

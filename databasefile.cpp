#include "databasefile.h"

DatabaseFile::DatabaseFile(QObject *parent)
    : QObject{parent}
{
}

bool DatabaseFile::open(const QString &path, QIODevice::OpenMode mode)
{
    file.setFileName(path);
    return file.open(mode);
}

bool DatabaseFile::readBool()
{
    char ch;
    file.read(&ch, 1);
    return ch != 0;
}

void DatabaseFile::writeBool(bool value)
{
    const char ch = value ? 1 : 0;
    file.write(&ch, 1);
}

int DatabaseFile::readInt()
{
    int val;
    file.read((char*)&val, sizeof(int));
    return val;
}

void DatabaseFile::writeInt(int value)
{
    file.write((char*)&value, sizeof(value));
}

qint64 DatabaseFile::readValue()
{
    qint64 val;
    file.read((char*)&val, sizeof(val));
    return val;
}

void DatabaseFile::writeValue(qint64 value)
{
    file.write((char*)&value, sizeof(value));
}

QString DatabaseFile::readString()
{
    static char buffer[1024];
    quint16 len;
    file.read((char*)&len, sizeof(len));
    file.read(buffer, len);
    buffer[len] = 0;
    return QString(buffer);
}

void DatabaseFile::writeString(const QString& str)
{
    QByteArray a = str.toUtf8();
    quint16 len = (quint16)a.size();
    file.write((char*)&len, sizeof(len));
    file.write(a);
}

QDate DatabaseFile::readDate()
{
    qint64 jd = readValue();
    return QDate::fromJulianDay(jd);
}

void DatabaseFile::writeDate(const QDate &date)
{
    qint64 jd = date.toJulianDay();
    writeValue(jd);
}

void DatabaseFile::writeWallets(const QList<Wallet> &wallets)
{
    writeInt(int(wallets.size()));
    for (const auto& wallet : wallets)
    {
        writeString(wallet.name);
        writeValue(wallet.initialValue);
        writeBool(wallet.isCredit);
    }
}

void DatabaseFile::readWallets(QList<Wallet> &wallets)
{
    wallets.clear();
    int n = readInt();
    for (int i = 0; i < n; ++i)
    {
        Wallet wallet;
        wallet.name = readString();
        wallet.initialValue = readValue();
        wallet.isCredit = readBool();
        wallets.append(wallet);
    }
}

void DatabaseFile::writeStringList(const QStringList& list)
{
    writeInt(int(list.size()));
    for (const auto& it : list)
    {
        writeString(it);
    }
}

void DatabaseFile::readStringList(QStringList& list)
{
    list.clear();
    const int n = readInt();
    for (int i = 0; i < n; ++i)
    {
        list << readString();
    }
}

Transaction DatabaseFile::readTransaction()
{
    Transaction tx;
    tx.date = readDate();
    tx.locationIndex = readInt();
    tx.walletIndex = readInt();
    tx.itemIndex = readInt();
    tx.num = readInt();
    tx.value = readValue();
    return tx;
}

void DatabaseFile::writeTransaction(const Transaction &tx)
{
    writeDate(tx.date);
    writeInt(tx.locationIndex);
    writeInt(tx.walletIndex);
    writeInt(tx.itemIndex);
    writeInt(tx.num);
    writeValue(tx.value);
}


void DatabaseFile::readTransactions(QList<Transaction> &list)
{
    list.clear();
    const int n = readInt();
    for (int i = 0; i < n; ++i)
    {
        list.append(readTransaction());
    }
}

void DatabaseFile::writeTransactions(const QList<Transaction> &list)
{
    const int n = list.size();
    writeInt(n);
    for (const auto& tx : list)
    {
        writeTransaction(tx);
    }
}

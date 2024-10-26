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
        writeValue(wallet.value);
        writeBool(wallet.external);
        writeDate(wallet.date);
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
        wallet.value = readValue();
        wallet.external = readBool();
        wallet.date = readDate();
        wallets.append(wallet);
    }
}

void DatabaseFile::writeTaggedNames(const QList<TaggedName> &arr)
{
    writeInt(int(arr.size()));
    for (const auto& it : arr)
    {
        writeString(it.name);
        writeInt(int(it.tagIndices.size()));
        for (const int i : it.tagIndices)
        {
            writeInt(i);
        }
    }
}

void DatabaseFile::readTaggedNames(QList<TaggedName> &arr)
{
    arr.clear();
    const int n = readInt();
    for (int i = 0; i < n; ++i)
    {
        TaggedName t;
        t.name = readString();
        const int nn = readInt();
        for (int ii = 0; ii < nn; ++ii)
        {
            t.tagIndices.append(readInt());
        }
        arr.append(t);
    }
}

void DatabaseFile::writeJournals(const QList<Journal> &journals)
{
    writeInt(int(journals.size()));
    for (const auto& journal : journals)
    {
        writeDate(journal.date);
        writeInt(journal.locationIndex);
        writeInt(journal.walletIndex);
        writeBool(journal.isDebit);
        writeInt(int(journal.entries.size()));
        for (const auto& entry : journal.entries)
        {
           writeInt(entry.itemIndex);
           writeInt(entry.num);
           writeValue(entry.value);
           writeValue(entry.balance);
        }
    }
}

void DatabaseFile::readJournals(QList<Journal> &journals)
{
    journals.clear();
    const int n = readInt();
    for (int i = 0; i < n; ++i)
    {
        Journal j;
        j.date = readDate();
        j.locationIndex = readInt();
        j.walletIndex = readInt();
        j.isDebit = readBool();
        const int nn = readInt();
        for (int ii = 0; ii < nn; ++ii)
        {
            JournalEntry e;
            e.itemIndex = readInt();
            e.num = readInt();
            e.value = readValue();
            e.balance = readValue();
            j.entries.append(e);
        }
        journals.append(j);
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

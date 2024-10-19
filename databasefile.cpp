#include "databasefile.h"

DatabaseFile::DatabaseFile(QObject *parent)
    : QObject{parent}
{
}

bool DatabaseFile::open(const QString &path, QIODeviceBase::OpenMode mode)
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
    char ch = value ? 0 : 1;
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

void DatabaseFile::writeWallets(const QList<Wallet> &wallets)
{
    writeInt(int(wallets.size()));
    for (const auto& wallet : wallets)
    {
        writeString(wallet.name);
        writeValue(wallet.value);
    }
}

void DatabaseFile::readWallets(QList<Wallet> &wallets)
{
    wallets.resize(readInt());
    for (auto& wallet : wallets)
    {
        wallet.name = readString();
        wallet.value = readValue();
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
    arr.resize(readInt());
    for (auto& it: arr)
    {
        it.name = readString();
        it.tagIndices.resize(readInt());
        for (auto& i : it.tagIndices)
        {
            i = readInt();
        }
    }
}

void DatabaseFile::writeJournals(const QList<Journal> &journals)
{
    writeInt(int(journals.size()));
    for (const auto& journal : journals)
    {
        writeInt(journal.date.toJulianDay());
        writeInt(journal.locationIndex);
        writeInt(journal.walletIndex);
        writeBool(journal.isDebit);
        writeInt(int(journal.entries.size()));
        for (const auto& entry : journal.entries)
        {
           writeInt(entry.itemIndex);
           writeInt(entry.num);
           writeValue(entry.value);
        }
    }
}

void DatabaseFile::readJournals(QList<Journal> &journals)
{
    int num = readInt();
    journals.resize(num);
    for (auto& journal : journals)
    {
        journal.date = QDate::fromJulianDay(readValue());
        journal.locationIndex = readInt();
        journal.walletIndex = readInt();
        journal.isDebit = readBool();
        journal.entries.resize(readInt());
        for (auto& entry : journal.entries)
        {
            entry.itemIndex = readInt();
            entry.num = readInt();
            entry.value = readValue();
        }


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
    list.resize(readInt());
    for (auto& it : list)
    {
        it = readString();
    }
}

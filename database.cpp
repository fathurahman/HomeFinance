#include <QDebug>
#include <QFile>
#include "database.h"

Database *db = nullptr;

namespace db_utils {
    bool readBool(QFile& file) {
        char ch;
        file.read(&ch, 1);
        return ch != 0;
    }

    void writeBool(QFile& file, bool value) {
        char ch = value ? 0 : 1;
        file.write(&ch, 1);
    }

    int readInt(QFile& file) {
        int val;
        file.read((char*)&val, sizeof(int));
        return val;
    }

    void writeInt(QFile& file, int value) {
        file.write((char*)&value, sizeof(value));
    }

    qint64 readValue(QFile& file)
    {
        qint64 val;
        file.read((char*)&val, sizeof(val));
        return val;
    }

    void writeValue(QFile& file, qint64 value) {
        file.write((char*)&value, sizeof(value));
    }

    QString readString(QFile& file)
    {
        static char buffer[1024];
        quint16 len;
        file.read((char*)&len, sizeof(len));
        file.read(buffer, len);
        buffer[len] = 0;
        return QString(buffer);
    }

    void writeString(QFile& file, const QString& str) {
        QByteArray a = str.toUtf8();
        quint16 len = (quint16)a.size();
        file.write((char*)&len, sizeof(len));
        file.write(a);
    }

} // namespace db_utils
using namespace db_utils;


Database::Database(QObject *parent)
    : QObject{parent}
{
    db = this;
}

int Database::getOrAddWallet(const QString &name)
{
    if (name.isEmpty()) {
        return -1;
    }
    const int n = walletDataList.size();
    for (int i = 0; i < n; ++i) {
        if (walletDataList[i].name == name) {
            return i;
        }
    }
    WalletData w;
    w.name = name;
    w.value = 0;
    walletDataList.append(w);
    return n;
}

int Database::getOrAddLocation(const QString& name)
{
    if (name.isEmpty()) {
        return -1;
    }
    const int n = locationNames.size();
    for (int i = 0; i < n; ++i) {
        if (locationNames[i] == name) {
            return i;
        }
    }
    locationNames.append(name);
    return n;
}

int Database::getOrAddItem(const QString& name)
{
    if (name.isEmpty()) {
        return -1;
    }
    const int n = itemNames.size();
    for (int i = 0; i < n; ++i) {
        if (itemNames[i] == name) {
            return i;
        }
    }
    itemNames.append(name);
    return n;
}

qint64 Database::totalValue() const
{
    qint64 value = 0;
    for (const auto& w : walletDataList) {
        value += w.value;
    }
    return value;
}

bool Database::load(const QString& path)
{
    int num;

    QFile file(path);
    if (file.open(QIODevice::ReadOnly) == false) {
        qDebug() << "ERROR: can not open file for reading:" << path;
        return false;
    }

    emit loading();

    num = readInt(file);
    walletDataList.resize(num);
    for (int i = 0; i < num; ++i) {
        WalletData& wallet = walletDataList[i];
        wallet.name = readString(file);
        wallet.value = readValue(file);
    }

    num = readInt(file);
    itemNames.resize(num);
    for (int i = 0; i < num; ++i) {
        itemNames[i] = readString(file);
    }

    num = readInt(file);
    locationNames.resize(num);
    for (int i = 0; i < num; ++i) {
        locationNames[i] = readString(file);
    }

    num = readInt(file);
    journalDataList.resize(num);
    for (int i = 0; i < num; ++i) {
        JournalData& j = journalDataList[i];
        j.dateTime = QDateTime::fromSecsSinceEpoch(readValue(file));
        j.location = readInt(file);
        j.wallet = readInt(file);
        j.isDebit = readBool(file);
        int en = readInt(file);
        j.entryDataList.resize(en);
        for (int ei = 0;  ei <  en; ++ei) {
            auto& e = j.entryDataList[ei];
            e.item = readInt(file);
            e.num = readInt(file);
            e.value = readValue(file);
        }
    }

    emit loaded();

    return true;
}

bool Database::save(const QString &path) const
{
    QFile file(path);
    if (file.open(QIODevice::WriteOnly) == false) {
        qDebug() << "ERROR: can not open file for writing : " << path;
        return false;
    }

    writeInt(file, walletDataList.size());
    for (const auto& w : walletDataList) {
        writeString(file, w.name);
        writeValue(file, w.value);
    }

    writeInt(file, itemNames.size());
    for (const auto& name : itemNames) {
        writeString(file, name);
    }

    writeInt(file, locationNames.size());
    for (const auto& name : locationNames) {
        writeString(file, name);
    }

    writeInt(file, journalDataList.size());
    for (const auto& j: journalDataList) {
        writeValue(file, j.dateTime.toSecsSinceEpoch());
        writeInt(file, j.location);
        writeInt(file, j.wallet);
        writeBool(file, j.isDebit);
        writeInt(file, j.entryDataList.size());
        for (const auto& e : j.entryDataList) {
            writeInt(file, e.item);
            writeInt(file, e.num);
            writeValue(file, e.value);
        }
    }

    return true;
}


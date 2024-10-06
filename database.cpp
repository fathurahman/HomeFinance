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

qint64 Database::totalValue() const
{
    qint64 value = 0;
    for (const auto& w : wallets) {
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
    wallets.resize(num);
    for (int i = 0; i < num; ++i) {
        Wallet& wallet = wallets[i];
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
    journals.resize(num);
    for (int i = 0; i < num; ++i) {
        Journal& j = journals[i];
        j.dateTime = QDateTime::fromSecsSinceEpoch(readValue(file));
        j.location = readInt(file);
        j.wallet = readInt(file);
        j.isDebit = readBool(file);
        int en = readInt(file);
        j.entries.resize(en);
        for (int ei = 0;  ei <  en; ++ei) {
            JournalEntry& e = j.entries[ei];
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

    writeInt(file, wallets.size());
    for (const auto& wallet : wallets) {
        writeString(file, wallet.name);
        writeValue(file, wallet.value);
    }

    writeInt(file, itemNames.size());
    for (const auto& name : itemNames) {
        writeString(file, name);
    }

    writeInt(file, locationNames.size());
    for (const auto& name : locationNames) {
        writeString(file, name);
    }

    writeInt(file, journals.size());
    for (const auto& j: journals) {
        writeValue(file, j.dateTime.toSecsSinceEpoch());
        writeInt(file, j.location);
        writeInt(file, j.wallet);
        writeBool(file, j.isDebit);
        writeInt(file, j.entries.size());
        for (const auto& e : j.entries) {
            writeInt(file, e.item);
            writeInt(file, e.num);
            writeValue(file, e.value);
        }
    }

    return true;
}


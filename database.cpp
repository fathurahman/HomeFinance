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

bool Database::hasWalletWithName(const QString &name) const
{
    for (const auto& w : m_walletDataList) {
        if (w.name == name) {
            return true;
        }
    }
    return false;
}

int Database::addWallet(const WalletData &data)
{
    if (data.name.isEmpty()) {
        return -1;
    }
    const int n = m_walletDataList.size();
    for (int i = 0; i < n; ++i) {
        if (m_walletDataList[i].name == data.name) {
            return i;
        }
    }
    m_walletDataList.append(data);
    m_activeWallet = n;
    if (data.value != 0) {
        m_totalValue += data.value;
        emit totalValueChanged();
    }
    return n;
}

QStringList Database::walletNames() const
{
    QStringList ret;
    for (const auto& w : m_walletDataList) {
        ret.append(w.name);
    }
    return ret;
}

QCompleter *Database::createItemNameCompleter(QObject *parent)
{
    if (m_itemNames.size() == 0) {
        return nullptr;
    }
    return new QCompleter(m_itemNames, parent);
}

QCompleter *Database::createLocationNameCompleter(QObject *parent)
{
    if (m_locationNames.size() == 0) {
        return nullptr;
    }
    return new QCompleter(m_locationNames, parent);
}

int Database::getOrAddWallet(const QString &name)
{
    if (name.isEmpty()) {
        return -1;
    }
    const int n = m_walletDataList.size();
    for (int i = 0; i < n; ++i) {
        if (m_walletDataList[i].name == name) {
            return i;
        }
    }
    WalletData w;
    w.name = name;
    w.value = 0;
    m_walletDataList.append(w);
    return n;
}

int Database::getOrAddLocation(const QString& name)
{
    if (name.isEmpty()) {
        return -1;
    }
    const int n = m_locationNames.size();
    for (int i = 0; i < n; ++i) {
        if (m_locationNames[i] == name) {
            return i;
        }
    }
    m_locationNames.append(name);
    return n;
}

int Database::getOrAddItem(const QString& name)
{
    if (name.isEmpty()) {
        return -1;
    }
    const int n = m_itemNames.size();
    for (int i = 0; i < n; ++i) {
        if (m_itemNames[i] == name) {
            return i;
        }
    }
    m_itemNames.append(name);
    return n;
}

int Database::addJournal(const JournalData &data)
{
    if (data.wallet < 0 || data.wallet >= m_walletDataList.size()) {
        return -1;
    }
    qint64 journalValue = 0;
    for (const auto& e : data.entryDataList) {
        journalValue += e.value;
    }
    if (journalValue <= 0) {
        return -1;
    }
    m_journalDataList.append(data);
    auto& d = m_journalDataList.last();
    auto& w = m_walletDataList[data.wallet];
    for (auto& e : d.entryDataList) {
        if (d.isDebit) {
            w.value += e.value;
            m_totalValue += e.value;
        } else {
            w.value -= e.value;
            m_totalValue -= e.value;
        }
        e.balance = w.value;
    }

    emit totalValueChanged();
    emit journalAdded();

    return m_journalDataList.size() - 1;
}

QList<TransactionPointer> Database::filterTransactions(const TransactionFilter &filter) const
{
    QList<TransactionPointer> ptrs;

    const bool hasLocationFilter = !filter.location.isEmpty();
    int specificLocation = -1;
    if (hasLocationFilter)  {
        specificLocation = m_locationNames.indexOf(filter.location);
    }

    const bool hasItemFilter = !filter.item.isEmpty();
    int specificItem = -1;
    if (hasItemFilter) {
        specificItem = m_itemNames.indexOf(filter.item);
    }

    const int journalNum = m_journalDataList.size();
    for (int journal = 0; journal < journalNum; ++journal) {

        const auto& j = m_journalDataList[journal];

        if (filter.year >= 0 && filter.year != j.dateTime.date().year()) {
            continue;
        }

        if (filter.month >= 0 && filter.month != j.dateTime.date().month()) {
            continue;
        }

        if (hasLocationFilter) {
            if (j.location < 0) {
                continue;
            }

            if (specificLocation >= 0) {
                if (j.location != specificLocation) {
                    continue;
                }
            } else {
                const auto& str = m_locationNames[j.location];
                if (str.contains(filter.location) == false) {
                    continue;
                }
            }
        }

        if (filter.wallet >= 0 && j.wallet != filter.wallet) {
            continue;
        }

        TransactionPointer ptr;
        ptr.journal = journal;
        for (int i = 0; i < j.entryDataList.size(); ++i) {
            ptr.entry = i;
            if (hasItemFilter) {
                const auto& e = j.entryDataList.at(i);
                if (specificItem) {
                    if (e.item != specificItem) {
                        continue;
                    }
                } else {
                    const auto& str = m_itemNames[e.item];
                    if (str.contains(filter.item) == false) {
                        continue;
                    }
                }
            }
            ptrs.append(ptr);
        }
    }

    return ptrs;
}

Transaction Database::transaction(const TransactionPointer& ptr) const
{
    const auto& j = m_journalDataList[ptr.journal];
    const auto& e = j.entryDataList[ptr.entry];
    Transaction tx;
    tx.dateTime = j.dateTime;
    tx.location = j.location;
    tx.wallet = j.wallet;
    tx.item = e.item;
    tx.num = e.num;
    tx.credit = j.isDebit ? 0 : e.value;
    tx.debit = j.isDebit ? e.value : 0;
    tx.balance = e.balance;
    return tx;
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
    m_walletDataList.resize(num);
    for (int i = 0; i < num; ++i) {
        WalletData& wallet = m_walletDataList[i];
        wallet.name = readString(file);
        wallet.value = readValue(file);
    }

    num = readInt(file);
    m_itemNames.resize(num);
    for (int i = 0; i < num; ++i) {
        m_itemNames[i] = readString(file);
    }

    num = readInt(file);
    m_locationNames.resize(num);
    for (int i = 0; i < num; ++i) {
        m_locationNames[i] = readString(file);
    }

    num = readInt(file);
    m_journalDataList.resize(num);
    for (int i = 0; i < num; ++i) {
        JournalData& j = m_journalDataList[i];
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

    writeInt(file, m_walletDataList.size());
    for (const auto& w : m_walletDataList) {
        writeString(file, w.name);
        writeValue(file, w.value);
    }

    writeInt(file, m_itemNames.size());
    for (const auto& name : m_itemNames) {
        writeString(file, name);
    }

    writeInt(file, m_locationNames.size());
    for (const auto& name : m_locationNames) {
        writeString(file, name);
    }

    writeInt(file, m_journalDataList.size());
    for (const auto& j: m_journalDataList) {
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


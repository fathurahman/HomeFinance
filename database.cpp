#include <QDebug>
#include <QFile>
#include "database.h"
#include "databasefile.h"

Database *db = nullptr;

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

QStringList Database::walletNames() const
{
    QStringList ret;
    for (const auto& wd : m_walletDataList)
    {
        ret.append(wd.name);
    }
    return ret;
}

QString Database::walletName(int index) const
{
    if (index < 0 || index >= m_walletDataList.size())
    {
        return QString();
    }
    return m_walletDataList[index].name;
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
    m_isModified = true;
    m_walletDataList.append(data);
    m_activeWallet = n;
    if (data.value != 0) {
        m_totalValue += data.value;
        emit totalValueChanged();
    }
    return n;
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
    m_isModified = true;
    WalletData w;
    w.name = name;
    w.value = 0;
    m_walletDataList.append(w);
    return n;
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
    m_isModified = true;
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
    m_isModified = true;
    m_itemNames.append(name);
    return n;
}

int Database::addJournal(const JournalData &data)
{
    if (data.wallet < 0 || data.wallet >= m_walletDataList.size())
    {
        return -1;
    }
    m_activeWallet = data.wallet;
    qint64 value = 0;
    for (const auto& e : data.entryDataList)
    {
        value += e.value;
    }
    if (value <= 0)
    {
        return -1;
    }
    m_isModified = true;
    m_journalDataList.append(data);
    auto& d = m_journalDataList.last();
    auto& w = m_walletDataList[data.wallet];
    for (auto& e : d.entryDataList)
    {
        value = d.isDebit ? e.value : -e.value;
        w.value += value;
        m_totalValue += value;
        e.balance = w.value;
    }
    emit totalValueChanged();
    emit journalAdded();
    return m_journalDataList.size() - 1;
}

TransactionFilter Database::normalizeFilter(const TransactionFilter& filter) const
{
    TransactionFilter f = filter;
    f.hasLocation = !f.location.isEmpty();
    f.specificLocation = f.hasLocation ? m_locationNames.indexOf(f.location) : -1;
    f.hasItem = !f.item.isEmpty();
    f.specificItem = f.hasItem ? m_itemNames.indexOf(f.item) : -1;
    return f;
}

bool Database::filterDateTime(const QDateTime& dt, const TransactionFilter& f) const
{
    const auto d = dt.date();
    if (f.year  >= 0 && f.year  != d.year())
        return false;
    if (f.month >= 0 && f.month != d.month())
        return false;
    if (f.day   >= 0 && f.day   != d.day())
        return false;
    return true;
}

bool Database::filterLocation(int loc, const TransactionFilter& f) const
{
    if (false == f.hasLocation)
        return true;
    if (f.specificLocation >= 0)
    {
        if (loc != f.specificLocation)
            return false;
    }
    else
    {
        const auto& s = m_locationNames[loc];
        if (s.contains(f.location) == false)
            return false;
    }
    return true;
}

bool Database::filterItem(int item, const TransactionFilter& f) const
{
    if (false == f.hasItem)
        return true;
    if (f.specificItem >= 0)
    {
        if (item != f.specificItem)
            return false;
    }
    else
    {
        const auto& s = m_itemNames[item];
        if (s.contains(f.item) == false)
            return false;
    }
    return true;
}

bool Database::filterJournalData(const JournalData& j, const TransactionFilter &f) const
{
    if (f.wallet >= 0 && j.wallet != f.wallet)
        return false;
    if (filterDateTime(j.dateTime, f) == false)
        return false;
    if (filterLocation(j.location, f) == false)
        return false;
    return true;
}

QList<TransactionPointer> Database::filterTransactions(const TransactionFilter &filter) const
{
    QList<TransactionPointer> ptrs;
    TransactionPointer ptr;
    TransactionFilter f = normalizeFilter(filter);
    const int n = m_journalDataList.size();
    for (int i = 0; i < n; ++i)
    {
        const auto& j = m_journalDataList[i];
        if (filterJournalData(j, f))
        {
            const int nn = j.entryDataList.size();
            for (int ii = 0; ii < nn; ++ii)
            {
                const auto& e = j.entryDataList.at(ii);
                if (filterItem(e.item, f))
                {
                    ptr.journal = i;
                    ptr.entry   = ii;
                    ptrs.append(ptr);
                }
            }
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
    DatabaseFile file;
    if (file.open(path, QIODevice::ReadOnly) == false)
    {
        qDebug() << "ERROR: can not open file for reading:" << path;
        return false;
    }
    m_lastFilePath = path;
    emit loading();

    int num = file.readInt();
    m_walletDataList.resize(num);
    m_totalValue = 0;
    for (int i = 0; i < num; ++i)
    {
        WalletData& wallet = m_walletDataList[i];
        wallet.name  = file.readString();
        wallet.value = file.readValue();
        m_totalValue += wallet.value;
    }

    num = file.readInt();
    m_itemNames.resize(num);
    for (int i = 0; i < num; ++i)
    {
        m_itemNames[i] = file.readString();
    }

    num = file.readInt();
    m_locationNames.resize(num);
    for (int i = 0; i < num; ++i)
    {
        m_locationNames[i] = file.readString();
    }

    num = file.readInt();
    m_journalDataList.resize(num);
    for (int i = 0; i < num; ++i)
    {
        JournalData& j = m_journalDataList[i];
        j.dateTime = QDateTime::fromSecsSinceEpoch(file.readValue());
        j.location = file.readInt();
        j.wallet   = file.readInt();
        j.isDebit  = file.readBool();
        int en = file.readInt();
        j.entryDataList.resize(en);
        for (int ei = 0;  ei <  en; ++ei)
        {
            auto& e = j.entryDataList[ei];
            e.item = file.readInt();
            e.num = file.readInt();
            e.value = file.readValue();
        }
    }

    m_isModified = false;
    emit loaded();

    return true;
}

bool Database::save(const QString &path) const
{
    DatabaseFile file;
    if (file.open(path, QIODevice::WriteOnly) == false) {
        qDebug() << "ERROR: can not open file for writing : " << path;
        return false;
    }

    m_lastFilePath = path;

    file.writeInt(m_walletDataList.size());
    for (const auto& w : m_walletDataList)
    {
        file.writeString(w.name);
        file.writeValue(w.value);
    }

    file.writeInt(m_itemNames.size());
    for (const auto& name : m_itemNames)
    {
        file.writeString(name);
    }

    file.writeInt(m_locationNames.size());
    for (const auto& name : m_locationNames)
    {
        file.writeString(name);
    }

    file.writeInt(m_journalDataList.size());
    for (const auto& j: m_journalDataList) {
        file.writeValue(j.dateTime.toSecsSinceEpoch());
        file.writeInt(j.location);
        file.writeInt(j.wallet);
        file.writeBool(j.isDebit);
        file.writeInt(j.entryDataList.size());
        for (const auto& e : j.entryDataList) {
            file.writeInt(e.item);
            file.writeInt(e.num);
            file.writeValue(e.value);
        }
    }

    m_isModified = false;
    return true;
}


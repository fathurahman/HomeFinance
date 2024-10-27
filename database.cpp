#include <QDebug>
#include <QFile>
#include "database.h"
#include "databasefile.h"

#define DB_VERSION 2

Database *db = nullptr;

Database::Database(QObject *parent)
    : QObject{parent}
{
    db = this;
}

bool Database::load(const QString& path)
{
    DatabaseFile file;
    if (file.open(path, QIODevice::ReadOnly) == false)
    {
        qDebug() << "ERROR: can not open file for reading:" << path;
        return false;
    }

    const int firstInt = file.readInt();
    if (firstInt != 0)
    {
        qWarning() << "The first int in database file is not zero";
        return false;
    }

    const int version = file.readInt();
    if (version != DB_VERSION)
    {
        qWarning() << "Invalid database file version, found:" << version << "expecting:" << DB_VERSION;
        return false;
    }

    file.readWallets(m_wallets);
    file.readStringList(m_itemNames);
    file.readStringList(m_locationNames);
    file.readTransactions(m_transactions);

    m_isModified = false;
    m_lastFilePath = path;

    m_activeWalletIndex = (m_wallets.size() == 0) ? -1 : 0;
    m_activeLocationIndex = (m_locationNames.size() == 0) ? -1 : 0;

    updateTransactionBalances();
    emit walletAdded();
    emit transactionAdded();

    return true;
}

bool Database::save(const QString &path) const
{
    DatabaseFile file;
    if (file.open(path, QIODevice::WriteOnly) == false) {
        qDebug() << "ERROR: can not open file for writing : " << path;
        return false;
    }

    file.writeInt(0);
    file.writeInt(DB_VERSION);
    file.writeWallets(m_wallets);
    file.writeStringList(m_itemNames);
    file.writeStringList(m_locationNames);
    file.writeTransactions(m_transactions);

    m_isModified = false;
    m_lastFilePath = path;

    return true;
}

int Database::walletsNum() const
{
    return int(m_wallets.size());
}

QStringList Database::walletNames() const
{
    QStringList ret;
    for (const auto& wallet : m_wallets)
    {
        ret.append(wallet.name);
    }
    return ret;
}

const Wallet* Database::wallet(int index) const
{
    if (index < 0 || index >= m_wallets.size())
    {
        return nullptr;
    }
    return &m_wallets[index];
}

const Wallet *Database::wallet(const QString &name) const
{
    for (const auto& wallet : m_wallets)
    {
        if (wallet.name == name)
        {
            return &wallet;
        }
    }
    return nullptr;
}

bool Database::addWallet(const Wallet& wallet)
{
    if (wallet.name.isEmpty())
    {
        return false;
    }    
    for (const auto& w : m_wallets)
    {
        if (wallet.name == w.name)
        {
            return false;
        }
    }
    m_wallets.append(wallet);
    updateTotalValue();
    m_isModified = true;
    emit walletAdded();
    return true;
}

QCompleter *Database::createItemNameCompleter(QObject *parent) const
{
    if (m_itemNames.isEmpty())
    {
        return nullptr;
    }
    return new QCompleter(m_itemNames, parent);
}

QCompleter *Database::createLocationNameCompleter(QObject *parent)
{    
    if (m_locationNames.isEmpty())
    {
        return nullptr;
    }
    return new QCompleter(m_locationNames, parent);
}

int Database::getOrAddWalletIndexByName(const QString &name)
{
    if (name.isEmpty())
    {
        return -1;
    }
    const int n = m_wallets.size();
    for (int i = 0; i < n; ++i)
    {
        if (m_wallets[i].name == name)
        {
            return i;
        }
    }
    m_wallets.append(Wallet(name));
    return m_wallets.size() - 1;
}

int Database::getOrAddLocationIndexByName(const QString &name)
{
    if (name.isEmpty())
    {
        return -1;
    }
    const int index = m_locationNames.indexOf(name);
    if (index >= 0)
    {
        return index;
    }
    m_locationNames.append(name);
    return m_locationNames.size() - 1;
}

int Database::getOrAddItemIndexByName(const QString &name)
{
    if (name.isEmpty())
    {
        return -1;
    }
    const int index = m_itemNames.indexOf(name);
    if (index >= 0)
    {
        return index;
    }
    m_itemNames.append(name);
    return m_itemNames.size() - 1;
}


bool Database::addJournal(const Journal &journal)
{
    Transaction tx;
    tx.date = journal.date;
    tx.locationIndex = getOrAddLocationIndexByName(journal.locationName);
    tx.walletIndex = getOrAddWalletIndexByName(journal.walletName);

    int i = -1;
    if (tx.date < QDate::currentDate() && m_transactions.isEmpty() == false)
    {
        for (i = m_transactions.size() - 1; i >= 0; --i)
        {
            if (m_transactions[i].date <= tx.date)
            {
                break;
            }
        }
        i += 1;
        if (i == m_transactions.size() - 1)
        {
            i = -1;
        }
    }

    if (tx.walletIndex < 0)
    {
        return false;
    }        
    Wallet& wallet = m_wallets[tx.walletIndex];
    const qint64 mult = journal.isDebit ? 1 : -1;
    for (const auto& e : journal.entries)
    {
        if (e.value == 0.0)
        {
            continue;
        }

        tx.itemIndex = getOrAddItemIndexByName(e.itemName);
        tx.num = e.num;
        tx.value = e.value * mult;

        if (i < 0)
        {
            m_transactions.append(tx);
        }
        else
        {
            m_transactions.insert(i, tx);
            i++;
        }

        wallet.value += tx.value;
        tx.balance = wallet.value;
    }


    updateTransactionBalances();

    m_isModified = true;

    m_activeWalletIndex = tx.walletIndex;
    m_activeLocationIndex = (tx.locationIndex >= 0) ? tx.locationIndex : m_activeLocationIndex;

    emit transactionAdded();

    return true;
}

bool Database::filterDate(const QDate& d, const TransactionFilter& f) const
{
    if (f.year  >= 0 && f.year  != d.year())
    {
        return false;
    }
    if (f.month >= 0 && f.month != d.month())
    {
        return false;
    }
    if (f.day   >= 0 && f.day   != d.day())
    {
        return false;
    }
    return true;
}

bool Database::filterLocationIndex(int index, const TransactionFilter& f) const
{
    if (!f.hasLocationName)
    {
        return true;
    }
    if (index < 0)
    {
        return false;
    }
    if (f.locationIndex >= 0)
    {
        return f.locationIndex == index;
    }
    return m_locationNames[index].contains(f.locationName, Qt::CaseInsensitive);
}

bool Database::filterItemIndex(int index, const TransactionFilter& f) const
{
    if (!f.hasItemName)
    {
        return true;
    }
    if (index < 0)
    {
        return false;
    }
    if (f.itemIndex >= 0)
    {
        return f.itemIndex == index;
    }
    return m_itemNames[index].contains(f.itemName, Qt::CaseInsensitive);
}

bool Database::filterKeyword(const Transaction &t, const TransactionFilter &f) const
{
    if (!f.hasKeyword)
    {
        return true;
    }
    if (t.walletIndex >= 0)
    {
        const QString& name = m_wallets[t.walletIndex].name;
        if (name.contains(f.keyword, Qt::CaseInsensitive))
        {
            return true;
        }
    }
    if (t.locationIndex >= 0)
    {
        const QString& name = m_locationNames[t.locationIndex];
        if (name.contains(f.keyword, Qt::CaseInsensitive))
        {
            return true;
        }
    }
    if (t.itemIndex >= 0)
    {
        const QString& name = m_itemNames[t.itemIndex];
        if (name.contains(f.keyword, Qt::CaseInsensitive))
        {
            return true;
        }
    }
    return false;
}

bool Database::filterTransaction(const Transaction &t, const TransactionFilter &f) const
{
    if (f.walletIndex >= 0 && t.walletIndex != f.walletIndex)
    {
        return false;
    }
    if (f.flow == 1 && t.value <= 0.0)
    {
        return false;
    }
    if (f.flow == 2 && t.value >= 0.0)
    {
        return false;
    }
    if (filterDate(t.date, f) == false)
    {
        return false;
    }
    if (filterLocationIndex(t.locationIndex, f) == false)
    {
        return false;
    }
    if (filterItemIndex(t.itemIndex, f) == false)
    {
        return false;
    }
    return filterKeyword(t, f);
}

QList<int> Database::filterTransactions(const TransactionFilter &filter) const
{
    QList<int> ret;
    const auto f = filter.process();
    const int n = m_transactions.size();
    for (int i = 0; i < n; ++i)
    {
        const auto& t = m_transactions[i];
        if (filterTransaction(t, f))
        {
            ret.append(i);
        }
    }
    return ret;
}


void Database::updateTotalValue(bool forceUpdate)
{
    qint64 totalValue = 0;
    for (const auto& wallet : m_wallets)
    {
        if (wallet.isCredit)
        {
            totalValue -= wallet.value;
        }
        else
        {
            totalValue += wallet.value;
        }
    }
    if (forceUpdate || totalValue != m_totalValue)
    {
        m_totalValue = totalValue;
        emit totalValueChanged();
    }
}

void Database::updateTransactionBalances()
{
    for (auto& wallet : m_wallets)
    {
        wallet.value = wallet.initialValue;
    }
    for (auto& tx : m_transactions)
    {
        auto& wallet = m_wallets[tx.walletIndex];
        wallet.value += tx.value;
        tx.balance = wallet.value;
    }
    updateTotalValue();
}


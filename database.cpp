#include <QDebug>
#include <QFile>
#include "database.h"
#include "databasefile.h"

#define DB_VERSION 1

Database *db = nullptr;

QString Transaction::locationName() const
{
    return db->locationName(locationIndex);
}

QString Transaction::itemName() const
{
    return db->itemName(itemIndex, num);
}

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

    int version = file.readInt();
    if (version != DB_VERSION)
    {
        return false;
    }

    emit loading();

    file.readWallets(m_wallets);
    file.readTaggedNames(m_items);
    file.readTaggedNames(m_locations);
    file.readJournals(m_journals);
    file.readStringList(m_tagNames);

    m_isModified = false;
    m_lastFilePath = path;
    m_activeWalletIndex = (m_wallets.size() == 0) ? -1 : 0;
    m_totalValue = 0;
    for (const auto& wallet : m_wallets)
    {
        m_totalValue += wallet.value;
    }

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

    file.writeWallets(m_wallets);
    file.writeTaggedNames(m_items);
    file.writeTaggedNames(m_locations);
    file.writeJournals(m_journals);
    file.writeStringList(m_tagNames);

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
    if (wallet.value > 0)
    {
        m_totalValue += wallet.value;
        emit totalValueChanged();
    }
    m_isModified = true;
    emit walletAdded();
    return true;
}

QString Database::itemName(int index, int num) const
{
    QString str;
    if (index >= 0 && index < m_items.size())
    {
        str = m_items[index].name;
    }
    if (num > 1)
    {
        str = QString("%1 x%2").arg(str).arg(num);
    }
    return str;
}

QCompleter *Database::createItemNameCompleter(QObject *parent) const
{
    if (m_items.isEmpty())
    {
        return nullptr;
    }
    QStringList names;
    for (const auto& item : m_items)
    {
        names.append(item.name);
    }
    return new QCompleter(names, parent);
}

QString Database::locationName(int index) const
{
    if (index < 0 || index >= m_locations.size())
    {
        return QString();
    }
    return m_locations[index].name;
}

QCompleter *Database::createLocationNameCompleter(QObject *parent)
{
    if (m_locations.isEmpty())
    {
        return nullptr;
    }
    QStringList names;
    for (const auto& loc : m_locations)
    {
        names.append(loc.name);
    }
    return new QCompleter(names, parent);
}

QCompleter *Database::createTagNameCompleter(QObject *parent)
{
    if (m_tagNames.isEmpty())
    {
        return nullptr;
    }
    return new QCompleter(m_tagNames, parent);
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
    const int n = m_locations.size();
    for (int i = 0; i < n; ++i)
    {
        if (m_locations[i].name == name)
        {
            return i;
        }
    }
    m_locations.append(TaggedName(name));
    return m_locations.size() - 1;
}

int Database::getOrAddItemIndexByName(const QString &name)
{
    if (name.isEmpty())
    {
        return -1;
    }
    const int n = m_items.size();
    for (int i = 0; i < n; ++i)
    {
        if (m_items[i].name == name)
        {
            return i;
        }
    }
    m_items.append(TaggedName(name));
    return m_items.size() - 1;
}


bool Database::addJournal(const JournalForm &form)
{
    const int walletIndex = getOrAddWalletIndexByName(form.walletName);
    if (walletIndex < 0)
    {
        return false;
    }
    Wallet& wallet = m_wallets[walletIndex];

    Journal journal;
        journal.date = form.date;
        journal.locationIndex = getOrAddLocationIndexByName(form.locationName);
        journal.walletIndex = walletIndex;
        journal.isDebit = form.isDebit;
        for (const auto& eform : form.entryForms)
        {
            JournalEntry e;
                e.itemIndex = getOrAddItemIndexByName(eform.itemName);
                e.num = eform.num;
                e.value = eform.value;
                    qint64 val = journal.isDebit ? e.value : -e.value;
                    if (val == 0)
                    {
                        continue;
                    }
                    wallet.value += val;
                    m_totalValue += val;
                e.balance = wallet.value;
            journal.entries.append(e);
        }
    if (journal.entries.isEmpty())
    {
        return false;
    }
    m_journals.append(journal);
    m_isModified = true;
    emit journalAdded();
    emit totalValueChanged();    
    return true;
}

TransactionFilter Database::normalizeFilter(const TransactionFilter& filter) const
{
    TransactionFilter f = filter;

    if (f.locationName.isEmpty())
    {
        f.hasLocationName = false;
        f.locationIndex = -1;
    }
    else
    {
        f.hasLocationName = true;
        f.locationIndex = -1;
        const int n = m_locations.size();
        for (int i = 0; i < n; ++i)
        {
            const auto& loc = m_locations[i];
            if (loc.name == f.locationName)
            {
                f.locationIndex = i;
                break;
            }
        }
    }

    if (f.itemName.isEmpty())
    {
        f.hasItemName = false;
        f.itemIndex = -1;
    }
    else
    {
        f.hasItemName = true;
        f.itemIndex = -1;
        const int n = m_items.size();
        for (int i = 0; i < n; ++i)
        {
            const auto& item = m_items[i];
            if  (item.name == f.itemName)
            {
                f.itemIndex = i;
                break;
            }
        }
    }

    f.tagIndex = -1;
    if (f.tagName.isEmpty() == false)
    {
        const int n = m_tagNames.size();
        for (int i = 0; i < n; ++i)
        {
            if (f.tagName == m_tagNames[i])
            {
                f.tagIndex = i;
                break;
            }
        }
    }

    return f;
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
    return f.locationName == m_locations[index].name;
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
    return m_items[index].name.contains(f.itemName);
}

bool Database::filterJournal(const Journal& j, const TransactionFilter &f) const
{
    if (filterDate(j.date, f) == false)
    {
        return false;
    }
    if (filterLocationIndex(j.locationIndex, f) == false)
    {
        return false;
    }
    if (f.walletIndex >= 0 && j.walletIndex != f.walletIndex)
    {
        return false;
    }
    if (f.flow == 1 && j.isDebit == false)
    {
        return false;
    }
    if (f.flow == 2 && j.isDebit)
    {
        return false;
    }
    return true;
}

bool Database::filterJournalEntry(const JournalEntry &entry, const TransactionFilter &filter) const
{
    if (filterItemIndex(entry.itemIndex, filter) == false)
    {
        return false;
    }
    if (filter.tagIndex >= 0)
    {
        if (entry.itemIndex < 0)
        {
            return false;
        }
        const auto& item = m_items[entry.itemIndex];
        if (item.tagIndices.contains(filter.tagIndex) == false)
        {
            return false;
        }
    }
    return true;
}

QList<TransactionPointer> Database::filterTransactions(const TransactionFilter &filter) const
{
    QList<TransactionPointer> ret;
    auto f = normalizeFilter(filter);
    const int n = m_journals.size();
    for (int i = 0; i < n; ++i)
    {
        const auto& j = m_journals[i];
        if (filterJournal(j, f))
        {
            const int nn = j.entries.size();
            for (int ii = 0; ii < nn; ++ii)
            {
                const auto& e = j.entries.at(ii);
                if (filterJournalEntry(e, f))
                {
                    ret.append(TransactionPointer(i, ii));
                }
            }
        }
    }
    return ret;
}

Transaction Database::transaction(const TransactionPointer& ptr) const
{
    const auto& j = m_journals[ptr.journalIndex];
    const auto& e = j.entries[ptr.entryIndex];
    Transaction tx;
    tx.date = j.date;
    tx.locationIndex = j.locationIndex;
    tx.itemIndex = e.itemIndex;
    tx.num = e.num;
    tx.walletIndex = j.walletIndex;
    tx.debit = j.isDebit ? e.value : 0;
    tx.credit = j.isDebit ? 0 : e.value;
    tx.balance = e.balance;
    return tx;
}


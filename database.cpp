#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include "database.h"


QJsonArray JournalEntry::save() const
{
    QJsonArray a;
    a.append(QJsonValue(name));
    a.append(QJsonValue(num));
    a.append(QJsonValue(value));
    return a;
}

void JournalEntry::load(const QJsonArray &a)
{
    name = a[0].toString();
    num = a[1].toInt();
    value = a[2].toInt();
}

QJsonArray JournalEntry::saveArray(const QList<JournalEntry> &entries)
{
    QJsonArray a;
    for (const auto& e : entries)
    {
        a.append(e.save());
    }
    return a;
}

QList<JournalEntry> JournalEntry::loadArray(const QJsonArray &a)
{
    QList<JournalEntry> entries;
    for (const auto ea : a)
    {
        JournalEntry e;
        e.load(ea.toArray());
        entries.push_back(e);
    }
    return entries;
}

void Journal::addEntry(const QString &name, int num, int value)
{
    JournalEntry e;
    e.name = name;
    e.num = num;
    e.value = value;
    entries << e;
}

QJsonArray Journal::save() const
{
    QJsonArray a;
    a.append(QJsonValue(timestamp.toSecsSinceEpoch()));
    a.append(QJsonValue(title));
    a.append(QJsonValue(isDebit));
    a.append(JournalEntry::saveArray(entries));
    return a;
}

void Journal::load(const QJsonArray &a)
{
    timestamp = QDateTime::fromSecsSinceEpoch(a[0].toInteger());
    title = a[1].toString();
    isDebit = a[2].toBool();
    entries = JournalEntry::loadArray(a[3].toArray());
}

QJsonArray Journal::saveArray(const QList<Journal> &journals)
{
    QJsonArray a;
    for (const auto& j : journals)
    {
        a.append(j.save());
    }
    return a;
}

QList<Journal> Journal::loadArray(const QJsonArray &a)
{
    QList<Journal> journals;
    for (const auto& ja : a)
    {
        Journal j;
        j.load(ja.toArray());
        journals.push_back(j);
    }
    return journals;
}


QJsonObject Wallet::save() const
{
    QJsonObject o;
    o.insert("name", QJsonValue(name));
    o.insert("value", QJsonValue(value));
    o.insert("journals", Journal::saveArray(journals));
    return o;
}

void Wallet::load(const QJsonObject &o)
{
    name = o.value("name").toString();
    value = o.value("value").toInt();
    journals = Journal::loadArray(o.value("journals").toArray());
}

void Wallet::debug() const
{
    qDebug() << "Wallet:" << name << "Value:" << value;
    for (const auto& j : journals)
    {
        qDebug() << "    Journal:" << j.title << "isDebit:" << j.isDebit << "Timestamp:" << j.timestamp.toString();
        for (const auto& e : j.entries)
        {
            qDebug() << "        " << e.name << "x" << e.num << e.value;
        }
    }
}


Database::Database(QObject *parent)
    : QObject{parent}
{

}


bool Database::load(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        return false;
    }
    auto json   = file.readAll();
    auto doc    = QJsonDocument::fromJson(json);
    auto a      = doc.array();
    const int n = a.size();
    if (n > 0)
    {
        QSet<QString> names;
        m_wallets.resize(n);
        for (int i = 0; i < n; ++i)
        {
            auto  o = a[i].toObject();
            auto& w = m_wallets[i];
            w.index = i;
            w.load(o);
            for (const auto& j : w.journals)
            {
                for (const auto& e : j.entries)
                {
                    names.insert(e.name);
                }
            }
        }
        m_journalEntryNames = names.values();
    }
    else
    {
        m_wallets.clear();
        m_journalEntryNames.clear();
    }
    return true;
}

bool Database::save(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
    {
        return false;
    }
    QJsonArray a;
    const int n = m_wallets.size();
    for (int i = 0; i < n; ++i)
    {
        auto o = m_wallets[i].save();
        a.append(o);
    }
    QJsonDocument doc(a);
    file.write(doc.toJson(QJsonDocument::Compact));
    return true;
}

int Database::addWallet(const QString &name, int value)
{
    for (const auto& it : m_wallets)
    {
        if (it.name == name)
            return -1;
    }
    Wallet w;
    w.index = m_wallets.size();
    w.name = name;
    w.value = value;
    m_wallets.push_back(w);
    return w.index;
}

int Database::addJournal(int walletIndex, const Journal &journal)
{
    // fail if the wallet index is not valid
    if (walletIndex < 0 || walletIndex >= m_wallets.size())
    {
        return -1;
    }
    // calculate journal value and add unique journal entry names
    int journalValue = 0;
    for (const auto& e : journal.entries)
    {
        journalValue += e.value;
        if (!m_journalEntryNames.contains(e.name))
        {
            m_journalEntryNames.push_back(e.name);
        }
    }
     // fail if the journal has no value
    if (journalValue <= 0)
    {
        return -1;
    }
    // add journal to wallet
    Wallet& wallet = m_wallets[walletIndex];
    wallet.journals.push_back(journal);
    // update wallet's value
    if (journal.isDebit)
    {
        wallet.value += journalValue;
    }
    else
    {
        wallet.value -= journalValue;
    }
    return wallet.journals.size() - 1;
}

QStringList Database::journalEntryNames(const QString& filter) const
{
    QStringList ret;
    for (const auto& name : m_journalEntryNames)
    {
        if (name.startsWith(filter))
        {
            ret << name;
        }
    }
    return ret;
}


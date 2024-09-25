#include "wallet.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

// JournalEntry
// ------------

QJsonValue JournalEntry::save() const
{
    QJsonArray a;
    a.append(QJsonValue(name));
    a.append(QJsonValue(num));
    a.append(QJsonValue(value));
    return a;
}

void JournalEntry::load(const QJsonValue &json)
{
    QJsonArray a = json.toArray();
    name = a[0].toString();
    num = a[1].toInt();
    value = a[2].toInt();
}

QJsonValue JournalEntry::saveArray(const QList<JournalEntry> &entries)
{
    QJsonArray a;
    for (const auto& e : entries)
    {
        a.append(e.save());
    }
    return a;
}

QList<JournalEntry> JournalEntry::loadArray(const QJsonValue &json)
{
    QJsonArray a = json.toArray();
    QList<JournalEntry> entries;
    for (const auto i : a)
    {
        JournalEntry e;
        e.load(i);
        entries.push_back(e);
    }
    return entries;
}

// Journal
// -------

int Journal::value() const
{
    int v = 0;
    for (const auto& e : entries)
    {
        v += e.value;
    }
    return v;
}

void Journal::addEntry(const QString &name, int num, int value)
{
    JournalEntry e;
    e.name = name;
    e.num = num;
    e.value = value;
    entries << e;
}

QJsonValue Journal::save() const
{
    QJsonArray a;
    a.append(QJsonValue(timestamp.toSecsSinceEpoch()));
    a.append(QJsonValue(title));
    a.append(QJsonValue(isDebit));
    a.append(JournalEntry::saveArray(entries));
    return a;
}

void Journal::load(const QJsonValue &json)
{
    QJsonArray a = json.toArray();
    timestamp = QDateTime::fromSecsSinceEpoch(a[0].toInteger());
    title = a[1].toString();
    isDebit = a[2].toBool();
    entries = JournalEntry::loadArray(a[3]);
}

QJsonValue Journal::saveArray(const QList<Journal> &journals)
{
    QJsonArray a;
    for (const auto& j : journals)
    {
        a.append(j.save());
    }
    return a;
}

QList<Journal> Journal::loadArray(const QJsonValue &json)
{
    QJsonArray a = json.toArray();
    QList<Journal> journals;
    for (const auto& i : a)
    {
        Journal j;
        j.load(i);
        journals.push_back(j);
    }
    return journals;
}

// Wallet
// ------

Wallet::Wallet(QObject *parent)
    : QObject{parent}
{

}

void Wallet::setName(const QString& name)
{
    if (m_name != name)
    {
        m_name = name;
        emit nameChanged(name);
    }
}

void Wallet::addJournal(const Journal& journal)
{
    const int jval = journal.value();
    if (jval <= 0)
        return;
    m_journals.push_back(journal);
    if (journal.isDebit)
    {
        m_value += jval;
    }
    else
    {
        m_value -= jval;
    }
    emit journalAdded();
}

QJsonValue Wallet::save() const
{
    QJsonArray a;
    a.append(QJsonValue(m_name));
    a.append(QJsonValue(m_value));
    a.append(Journal::saveArray(m_journals));
    return a;
}

void Wallet::load(const QJsonValue& json)
{
    QJsonArray a = json.toArray();
    m_name = a[0].toString();
    m_value = a[1].toInt();
    m_journals = Journal::loadArray(a[2]);
    emit loaded();
}

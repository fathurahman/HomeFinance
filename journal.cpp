
#include "application.h"
#include <QJsonArray>
#include <QJsonObject>

Journal::Journal(QObject *parent)
    : QObject{parent}
{

}

int Journal::addEntry(const QString &name, int quantity, qint64 value)
{
    JournalEntry e;
    e.id = app->db()->addJournalEntryName(name);
    e.quantity = quantity;
    e.value = value;
    m_entries.append(e);
    return m_entries.size() - 1;
}

QString Journal::entryName(int index) const
{
    const auto& e = m_entries[index];
    QString name = app->db()->getJournalEntryName(e.id);
    if (e.quantity > 1)
    {
        return QString("%1 x%2").arg(name).arg(e.quantity);
    }
    return name;
}

int Journal::entryQuantity(int index) const
{
    const auto& e = m_entries[index];
    return e.quantity;
}

qint64 Journal::entryValue(int index) const
{
    const auto& e = m_entries[index];
    return e.value;
}

qint64 Journal::value() const
{
    qint64 v = 0;
    for (const auto& e : m_entries)
    {
        v += e.value;
    }
    return v;
}

QJsonValue Journal::save() const
{
    QJsonArray ret;
    ret.append(QJsonValue(m_title));
    ret.append(QJsonValue(m_dateTime.toSecsSinceEpoch()));
    ret.append(QJsonValue(m_isDebit));
    QJsonArray a;
    for (const auto& e : m_entries)
    {
        a.append(e.save());
    }
    ret.append(a);
    return ret;
}

void Journal::load(const QJsonValue &json)
{
    QJsonArray ret = json.toArray();
    m_title = ret[0].toString();
    m_dateTime = QDateTime::fromSecsSinceEpoch(ret[1].toInteger());
    m_isDebit = ret[2].toBool();
    QJsonArray a = ret[3].toArray();
    m_entries.clear();
    for (const auto& v : a)
    {
        JournalEntry e;
        e.load(v);
        m_entries.append(e);
    }
}

void Journal::setDateTime(const QDateTime &dt)
{
    m_dateTime = dt;
}

void Journal::setIsDebit(bool value)
{
    m_isDebit = value;
}

void Journal::pushEntry()
{
    JournalEntry e;
    m_entries.append(e);
}

void Journal::setEntryName(int index, const QString &name)
{
    auto& e = m_entries[index];
    e.id = app->db()->addJournalEntryName(name);
}

void Journal::setEntryQuantity(int index, int quantity)
{
    auto& e = m_entries[index];
    e.quantity = quantity;
}

void Journal::setEntryValue(int index, int value)
{
    auto& e = m_entries[index];
    e.value = value;
}

void Journal::removeEntry(int index)
{
    auto& e = m_entries[index];
    e.id = -1;
}

void Journal::cleanupEntries()
{
    QList<JournalEntry> a;
    for (const auto& e : m_entries)
    {
        if (e.id >= 0)
        {
            a.append(e);
        }
    }
    m_entries = a;
}


QJsonValue JournalEntry::save() const
{
    QJsonArray a;
    a.append(QJsonValue(id));
    a.append(QJsonValue(quantity));
    a.append(QJsonValue(value));
    return a;
}

void JournalEntry::load(const QJsonValue &json)
{
    QJsonArray a = json.toArray();
    id = a[0].toInt();
    quantity = a[1].toInt();
    value = a[2].toInteger();
}

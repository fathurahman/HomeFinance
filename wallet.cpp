#include "wallet.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

Wallet::Wallet(QObject *parent)
    : QObject{parent}
{

}

void Wallet::addJournal(Journal *journal)
{
    if (!journal || m_journals.contains(journal))
    {
        return;
    }
    journal->setParent(this);
    m_journals.append(journal);
    emit journalAdded();
    return;
}

int Wallet::transactionNum() const
{
    int n = 0;
    for (const auto* journal : m_journals)
    {
        n += journal->entriesNum();
    }
    return n;
}

Transaction Wallet::transaction(int index) const
{
    int i = index;
    for (const auto* journal : m_journals)
    {
        const int n = journal->entriesNum();
        if (i < n)
        {
            Transaction tx;
            tx.timestamp = journal->dateTime();
            tx.name = journal->entryName(i);
            if (journal->isDebit())
            {
                tx.debit = journal->entryValue(i);
                tx.credit = 0;
            }
            else
            {
                tx.debit = 0;
                tx.credit = journal->entryValue(i);
            }
            tx.desc = journal->title();
            return tx;
        }
        else
        {
            i -= n;
        }
    }
    return Transaction();
}

QJsonValue Wallet::save() const
{
    QJsonObject o;
    o.insert("name", QJsonValue(m_name));
    o.insert("value", QJsonValue(m_value));
    QJsonArray a;
    for (const auto* journal : m_journals)
    {
        a.append(journal->save());
    }
    o.insert("journals", a);
    return o;
}

void Wallet::load(const QJsonValue &json)
{
    QJsonObject o = json.toObject();
    m_name = o.value("name").toString();
    m_value = o.value("value").toInt();
    for (auto* journal : m_journals)
    {
        delete journal;
    }
    m_journals.clear();
    QJsonArray a = o.value("journals").toArray();
    for (const auto v : a)
    {
        auto* journal = new Journal();
        journal->load(v);
        m_journals.append(journal);
    }
    emit loaded();
}

void Wallet::setName(const QString& name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged(name);
    }
}

void Wallet::setValue(int value)
{
    m_value = value;
}

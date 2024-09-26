#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include "database.h"


Database::Database(QObject *parent)
    : QObject{parent}
{

}

Wallet* Database::addWallet(const QString &name, int value)
{
    Wallet* wallet = new Wallet(this);
    wallet->setName(name);
    wallet->setValue(value);
    m_wallets.append(wallet);
    return wallet;
}

int Database::getJournalEntryId(const QString &name) const
{
    if (name.isEmpty())
    {
        return -1;
    }
    return m_journalEntryNames.indexOf(name);
}

QString Database::getJournalEntryName(int id) const
{
    if (id < 0)
    {
        return QString();
    }
    return m_journalEntryNames[id];
}

int Database::addJournalEntryName(const QString &name)
{
    int id = getJournalEntryId(name);
    if (id < 0)
    {
        id = m_journalEntryNames.size();
        m_journalEntryNames.append(name);
    }
    return id;
}


bool Database::load(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        return false;
    }
    emit loading();
    auto json   = file.readAll();
    auto doc    = QJsonDocument::fromJson(json);
    auto o      = doc.object();
    loadJournalEntryNames(o.value("names"));
    loadWallets(o.value("wallets"));
    emit loaded();
    return true;
}

bool Database::save(const QString& path) const
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
    {
        return false;
    }
    QJsonObject o;
    o.insert("names", saveJournalEntryNames());
    o.insert("wallets", saveWallets());
    QJsonDocument doc(o);
    auto json = doc.toJson(QJsonDocument::Compact);
    file.write(json);
    return true;
}

void Database::loadJournalEntryNames(const QJsonValue &json)
{
    m_journalEntryNames.clear();
    QJsonArray a = json.toArray();
    for (const auto& v : a)
    {
        const QString name = v.toString();
        m_journalEntryNames.append(name);
    }
}

void Database::loadWallets(const QJsonValue &json)
{
    for (auto* wallet : m_wallets)
    {
        delete wallet;
    }
    m_wallets.clear();
    QJsonArray a = json.toArray();
    for (const auto& v : a)
    {
        auto* w = new Wallet(this);
        w->load(v);
        m_wallets.append(w);
    }
}

QJsonValue Database::saveJournalEntryNames() const
{
    QJsonArray a;
    for (const auto& name : m_journalEntryNames)
    {
        a.append(QJsonValue(name));
    }
    return a;
}

QJsonValue Database::saveWallets() const
{
    QJsonArray a;
    for (const auto* w : m_wallets)
    {
        a.append(w->save());
    }
    return a;
}




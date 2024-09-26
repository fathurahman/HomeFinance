#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QStringList>
#include "wallet.h"

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = nullptr);

    inline int walletsNum() const { return m_wallets.size(); }
    Wallet* wallet(int index) const { return m_wallets.at(index); }
    Wallet* addWallet(const QString& name, int value);

    int getJournalEntryId(const QString& name);
    QString getJournalEntryName(int id);
    
    bool load(const QString& path);
    bool save(const QString& path);

signals:
    void loading();
    void loaded();

private:
    QList<Wallet*> m_wallets;
    QStringList m_journalEntryNames;

};

#endif // DATABASE_H

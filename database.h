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
    inline Wallet* wallet(int index) const { return m_wallets.at(index); }
    Wallet* addWallet(const QString& name, int value);

    inline const QStringList& journalEntryNames() const { return m_journalEntryNames; }
    int getJournalEntryId(const QString& name) const;
    QString getJournalEntryName(int id) const;
    int addJournalEntryName(const QString& name);
    
    bool load(const QString& path);
    bool save(const QString& path) const;


signals:
    void loading();
    void loaded();

private:
    QList<Wallet*> m_wallets;
    QStringList m_journalEntryNames;

private:
    void loadJournalEntryNames(const QJsonValue& json);
    void loadWallets(const QJsonValue& json);
    QJsonValue saveJournalEntryNames() const;
    QJsonValue saveWallets() const;
};


#endif // DATABASE_H

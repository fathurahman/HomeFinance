#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QStringList>
#include <QList>
#include <QSet>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>


struct JournalEntry
{
    QString name;
    int num;
    int value;

    QJsonArray save() const;
    void load(const QJsonArray &a);

    static QJsonArray saveArray(const QList<JournalEntry>& entries);
    static QList<JournalEntry> loadArray(const QJsonArray& a);
};

struct Journal
{
    QDateTime timestamp;
    QString title;
    bool isDebit;
    QList<JournalEntry> entries;

    void addEntry(const QString& name, int num, int value);

    QJsonArray save() const;
    void load(const QJsonArray& a);

    static QJsonArray saveArray(const QList<Journal>& journals);
    static QList<Journal> loadArray(const QJsonArray& a);
};

struct Wallet
{
    int index;
    QString name;
    int value;
    QList<Journal> journals;

    QJsonObject save() const;
    void load(const QJsonObject& o);

    void debug() const;
};


class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = nullptr);

    bool load(const QString& path);
    bool save(const QString& path);

    inline int walletsNum() const { return m_wallets.size(); }
    const Wallet& wallet(int index) { return m_wallets.at(index); }
    int addWallet(const QString& name, int value);
    int addJournal(int walletIndex, const Journal& journal);

    inline const QStringList& journalEntryNames() const { return m_journalEntryNames; }
    QStringList journalEntryNames(const QString& filter) const;
    
signals:

private:

    QList<Wallet> m_wallets;
    QStringList m_journalEntryNames;

};

#endif // DATABASE_H

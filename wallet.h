#ifndef WALLET_H
#define WALLET_H

#include <QObject>
#include <QJsonValue>

struct JournalEntry
{
    QString name;
    int num;
    int value;

    QJsonArray save() const;
    void load(const QJsonArray &a);

    static QJsonValue saveArray(const QList<JournalEntry>& entries);
    static QList<JournalEntry> loadArray(const QJsonValue& json);
};

struct Journal
{
    QDateTime timestamp;
    QString title;
    bool isDebit;
    QList<JournalEntry> entries;

    int value() const;
    void addEntry(const QString& name, int num, int value);

    QJsonValue save() const;
    void load(const QJsonValue& json);

    static QJsonValue saveArray(const QList<Journal>& journals);
    static QList<Journal> loadArray(const QJsonValue& json);
};


class Wallet : public QObject
{
    Q_OBJECT
public:
    explicit Wallet(QObject *parent = nullptr);

    inline const QString& name() const { return m_name; }
    void setName(const QString& name);

    inline int value() const { return m_value; }

    inline int journalNum() { return m_journals.size(); }
    inline const Journal& journal(int index) { return m_journals.at(index); }
    void addJournal(const Journal& journal);

    QJsonValue save() const;
    void load(const QJsonValue& json);

signals:
    void loaded();
    void nameChanged(QString name);
    void journalAdded();

private:
    QString m_name;
    int m_value = 0;
    QList<Journal> m_journals;
};

#endif // WALLET_H

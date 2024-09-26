#ifndef WALLET_H
#define WALLET_H

#include <QObject>
#include <QJsonValue>
#include "journal.h"

struct Transaction
{
    QDateTime timestamp;
    QString name;
    int debit = 0;
    int credit = 0;
    QString desc;
};

class Wallet : public QObject
{
    Q_OBJECT
public:
    explicit Wallet(QObject *parent = nullptr);

    inline const QString& name() const { return m_name; }
    inline int value() const { return m_value; }

    inline int journalNum() const { return m_journals.size(); }
    inline Journal* journal(int index) const;
    void addJournal(Journal* journal);

    int transactionNum() const;
    Transaction transaction(int index) const;

    QJsonValue save() const;
    void load(const QJsonValue& json);

public slots:
    void setName(const QString& name);
    void setValue(int value);

signals:    
    void loaded();
    void nameChanged(QString name);
    void journalAdded();

private:
    QString m_name;
    int m_value = 0;
    QList<Journal*> m_journals;
};

#endif // WALLET_H

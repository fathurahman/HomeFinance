#ifndef JOURNAL_H
#define JOURNAL_H

#include <QObject>
#include <QDateTime>
#include <QList>

struct JournalEntry
{
    int id;
    int quantity;
    int value;
};


class Journal : public QObject
{
    Q_OBJECT
public:
    explicit Journal(QObject *parent = nullptr);

    inline QString title() const { return m_title; }
    inline QDateTime dateTime() const { return m_dateTime; }
    inline bool isDebit() const { return m_isDebit; }

    int entriesNum() const;
    int addEntry(const QString& name, int quantity, int value);
    QString entryName(int index) const;
    int entryQuantity(int index) const;
    int entryValue(int index) const;

    int value() const;

    QJsonValue save() const;
    void load(const QJsonValue& json);

signals:

public slots:
    void setDateTime(const QDateTime& dt);
    void setIsDebit(bool value);
    void pushEntry();
    void setEntryName(int index, const QString& name);
    void setEntryQuantity(int index, int quantity);
    void setEntryValue(int index, int value);
    void removeEntry(int index);

private:
    QString m_title;
    QDateTime m_dateTime;
    int m_isDebit;
    QList<JournalEntry> entries;

private:

    QString entryIdToName(int id) const;
};

#endif // JOURNAL_H

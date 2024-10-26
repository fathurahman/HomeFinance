#ifndef DATABASEFILE_H
#define DATABASEFILE_H

#include <QObject>
#include <QFile>
#include "datatypes.h"

class DatabaseFile : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseFile(QObject *parent = nullptr);

    bool open(const QString& path, QIODevice::OpenMode mode);

    bool readBool();
    void writeBool(bool value);

    int readInt();
    void writeInt(int value);

    qint64 readValue();
    void writeValue(qint64 value);

    QString readString();
    void writeString(const QString& str);

    QDate readDate();
    void writeDate(const QDate& date);

    void writeWallets(const QList<Wallet>& wallets);
    void readWallets(QList<Wallet>& wallets);

    void writeTaggedNames(const QList<TaggedName>& taggedNames);
    void readTaggedNames(QList<TaggedName>& taggedNames);

    void writeJournals(const QList<Journal>& journals);
    void readJournals(QList<Journal>& journals);

    void writeStringList(const QStringList& list);
    void readStringList(QStringList& list);


private:
    QFile file;
};

#endif // DATABASEFILE_H

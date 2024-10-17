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

    bool open(const QString& path, QIODeviceBase::OpenMode mode);

    bool readBool();
    void writeBool(bool value);

    int readInt();
    void writeInt(int value);

    qint64 readValue();
    void writeValue(qint64 value);

    QString readString();
    void writeString(const QString& str);

    void writeWallets(const QList<Wallet>& wallets);
    void readWallets(QList<Wallet>& wallets);

    void writeNameAndTags(const QList<NameAndTags>& arr);
    void readNameAndTags(QList<NameAndTags>& arr);

    void writeJournals(const QList<Journal>& journals);
    void readJournals(QList<Journal>& journals);


private:
    QFile file;
};

#endif // DATABASEFILE_H

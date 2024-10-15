#include "databasefile.h"

DatabaseFile::DatabaseFile(QObject *parent)
    : QObject{parent}
{
}

bool DatabaseFile::open(const QString &path, QIODeviceBase::OpenMode mode)
{
    file.setFileName(path);
    return file.open(mode);
}

bool DatabaseFile::readBool()
{
    char ch;
    file.read(&ch, 1);
    return ch != 0;
}

void DatabaseFile::writeBool(bool value)
{
    char ch = value ? 0 : 1;
    file.write(&ch, 1);
}

int DatabaseFile::readInt()
{
    int val;
    file.read((char*)&val, sizeof(int));
    return val;
}

void DatabaseFile::writeInt(int value)
{
    file.write((char*)&value, sizeof(value));
}

qint64 DatabaseFile::readValue()
{
    qint64 val;
    file.read((char*)&val, sizeof(val));
    return val;
}

void DatabaseFile::writeValue(qint64 value)
{
    file.write((char*)&value, sizeof(value));
}

QString DatabaseFile::readString()
{
    static char buffer[1024];
    quint16 len;
    file.read((char*)&len, sizeof(len));
    file.read(buffer, len);
    buffer[len] = 0;
    return QString(buffer);
}

void DatabaseFile::writeString(const QString& str)
{
    QByteArray a = str.toUtf8();
    quint16 len = (quint16)a.size();
    file.write((char*)&len, sizeof(len));
    file.write(a);
}

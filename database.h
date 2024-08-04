#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include "data.h"

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = nullptr);

    int getItemsNum() const;


signals:

};

#endif // DATABASE_H

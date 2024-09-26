
#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>
#include <QApplication>
#include "database.h"

class Application : public QApplication
{
    Q_OBJECT
public:
    explicit Application(int &argc, char **argv);

    inline Database *db() const { return m_db; }

signals:

private:
    Database *m_db;
};

extern Application *app;

#endif // APPLICATION_H

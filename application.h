
#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>
#include <QApplication>
#include <QValidator>
#include <QLineEdit>
#include "database.h"

class Application : public QApplication
{
    Q_OBJECT
public:
    explicit Application(int &argc, char **argv);

    inline Database *db() const { return m_db; }

    inline QValidator* numValidator() const { return m_numValidator; }
    QLineEdit* createNumLineEdit(qint64 value = 0, Qt::Alignment align = Qt::AlignLeft) const;

signals:

private:
    Database *m_db;
    QValidator* m_numValidator;
};

extern Application *app;

#endif // APPLICATION_H

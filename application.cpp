#include "application.h"
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSettings>
#include "database.h"

Application *app = nullptr;

Application::Application(int &argc, char **argv)
    : QApplication{argc, argv}
{    
    app = this;

    m_db = new Database(this);
    QSettings s;
    QString path = s.value("LastFilePath", "").toString();
    if (path.isEmpty() == false && path.endsWith(".hfdb")) {
        m_db->load(path);
    }

    QRegularExpression re("[0-9]*");
    m_numValidator = new QRegularExpressionValidator(re, this);
}

QLineEdit *Application::createNumLineEdit(qint64 value, Qt::Alignment align) const
{
    auto* w = new QLineEdit;
    w->setText(QString("%1").arg(value));
    w->setValidator(m_numValidator);
    w->setAlignment(align);
    return w;
}


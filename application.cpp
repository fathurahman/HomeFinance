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

    QApplication::setOrganizationName("Hacknpaint");
    QApplication::setApplicationName("HomeFinance");
    QApplication::setApplicationVersion("1.0.0");

    m_db = new Database(this);
    QSettings s;
    QString path = s.value("LastFilePath", "").toString();
    if (path.isEmpty() == false && path.endsWith(FILE_EXT))
    {
        m_db->load(path);
    }

    m_ttm = new TransactionTableModel(this);

    QRegularExpression re("^[-+]?[0-9]*");
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


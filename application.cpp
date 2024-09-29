
#include "application.h"
#include <QRegularExpression>
#include <QRegularExpressionValidator>

Application *app = nullptr;

Application::Application(int &argc, char **argv)
    : QApplication{argc, argv}
{    
    app = this;
    m_db = new Database(this);

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


#ifndef ADDWALLETDIALOG_H
#define ADDWALLETDIALOG_H

#include <QDialog>
#include <QLineEdit>

class AddWalletDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddWalletDialog(QWidget *parent = nullptr);

    QString name() const { return ui_name->text(); }
    qint64 value() const { return ui_value->text().toLongLong(); }

signals:

private:
    QLineEdit* ui_name;
    QLineEdit* ui_value;

};

#endif // ADDWALLETDIALOG_H

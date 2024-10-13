#ifndef ADDWALLETDIALOG_H
#define ADDWALLETDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include "database.h"

class AddWalletDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddWalletDialog(QWidget *parent = nullptr);

    WalletData walletData() const;

signals:    

private:
    QLineEdit* ui_name;
    QLineEdit* ui_value;
    QPushButton* ui_addButton;

private slots:
    void onNameEdited(const QString& text);

};

#endif // ADDWALLETDIALOG_H

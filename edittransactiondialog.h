#ifndef EDITTRANSACTIONDIALOG_H
#define EDITTRANSACTIONDIALOG_H

#include <QDialog>
#include <QDateEdit>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include "datatypes.h"


class EditTransactionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit EditTransactionDialog(int index, QWidget *parent = nullptr);

    Transaction transaction() const;

signals:

private:
    int m_index;

    QDateEdit* ui_date;
    QLineEdit* ui_locationName;
    QComboBox* ui_type;
    QLineEdit* ui_itemName;
    QSpinBox* ui_quantity;
    QLineEdit* ui_value;
    QComboBox* ui_walletName;

};

#endif // EDITTRANSACTIONDIALOG_H

﻿#ifndef ADDJOURNALDIALOG_H
#define ADDJOURNALDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QList>
#include <QVBoxLayout>
#include <QDateEdit>
#include <QComboBox>
#include <QCompleter>
#include "datatypes.h"

struct JournalRow
{
    QLineEdit* name;
    QSpinBox* quantity;
    QLineEdit* value;
};

class AddJournalDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddJournalDialog(bool isDebit, QWidget *parent = nullptr);

    JournalForm journalForm() const;

signals:

private:
    QDateEdit* ui_date;
    QLineEdit* ui_location;
    QComboBox* ui_wallet;
    QComboBox* ui_type;
    QVBoxLayout* ui_rowBox;
    QList<JournalRow> ui_rows;

    QCompleter* m_itemNameCompleter;

private:
    void addRow();

};

#endif // ADDJOURNALDIALOG_H

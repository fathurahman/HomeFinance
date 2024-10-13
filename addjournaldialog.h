#ifndef ADDJOURNALDIALOG_H
#define ADDJOURNALDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QList>
#include <QVBoxLayout>
#include <QDateTimeEdit>
#include <QComboBox>
#include <QCompleter>
#include "database.h"

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

    JournalData journalData() const;

signals:

private:
    QDateTimeEdit* ui_dateTime;
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

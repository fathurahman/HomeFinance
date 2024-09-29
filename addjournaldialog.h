#ifndef ADDJOURNALDIALOG_H
#define ADDJOURNALDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QList>
#include <QVBoxLayout>
#include <QDateTimeEdit>
#include <QComboBox>
#include "journal.h"

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


    Journal* makeJournal() const;

signals:

private:
    bool m_isDebit;
    QDateTimeEdit* ui_dateTime;
    QComboBox* ui_wallet;
    QLineEdit* ui_title;
    QVBoxLayout* ui_rowBox;
    QList<JournalRow> ui_rows;

private:
    void addRow();

};

#endif // ADDJOURNALDIALOG_H

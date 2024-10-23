#ifndef VIEWWALLETSDIALOG_H
#define VIEWWALLETSDIALOG_H

#include <QWidget>
#include <QDialog>

class QHBoxLayout;
class QVBoxLayout;

class ViewWalletsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ViewWalletsDialog(QWidget *parent = nullptr);

private:
    class QVBoxLayout* ui_rows;

private:
    QHBoxLayout* makeRow(const QString& name, qint64 value);
    QVBoxLayout* makeRows();
};

#endif // VIEWWALLETSDIALOG_H

#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H

#include <QWidget>
#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include "datatypes.h"

class FilterWidget : public QWidget
{
	Q_OBJECT
public:
	FilterWidget(QWidget* parent = nullptr);

private:
    QSpinBox* ui_year;
    QSpinBox* ui_month;
    QSpinBox* ui_day;
    QComboBox* ui_wallet;
    QComboBox* ui_flow;
    QLineEdit* ui_keyword;

private slots:
    void updateWallet();
    void resetFilter();
    void applyFilter();
};

#endif

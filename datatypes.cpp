#include "datatypes.h"
#include "database.h"

QString Transaction::locationName() const
{
    return (locationIndex >= 0) ? db->locationNames()[locationIndex] : QString();
}

QString Transaction::walletName() const
{
    return (walletIndex >= 0) ? db->wallet(walletIndex)->name : QString();
}

QString Transaction::itemName() const
{
    QString name = (itemIndex >= 0) ? db->itemNames()[itemIndex] : QString();
    if (name.isEmpty())
    {
        return QString();
    }
    return (num > 1) ? QString("%1 x%2").arg(name).arg(num) : name;
}

qint64 Transaction::debit() const
{
    return (value > 0.0) ? value : 0.0;
}

qint64 Transaction::credit() const
{
    return (value < 0.0) ? -value : 0.0;
}

TransactionFilter TransactionFilter::process() const
{
    TransactionFilter f = *this;

    f.isEmpty = ((3 + year + month + day) == 0) && (walletIndex < 0) && (flow == TXF_FLOW_NONE);

    if (f.locationName.isEmpty())
    {
        f.hasLocationName = false;
        f.locationIndex = -1;
    }
    else
    {
        f.hasLocationName = true;
        f.locationIndex = db->locationNames().indexOf(f.locationName);
        f.isEmpty = false;
    }

    if (f.itemName.isEmpty())
    {
        f.hasItemName = false;
        f.itemIndex = -1;
    }
    else
    {
        f.hasItemName = true;
        f.itemIndex = db->itemNames().indexOf(f.itemName);
        f.isEmpty = false;
    }

    f.hasKeyword = false;
    if (f.keyword.isEmpty() == false)
    {
        if (f.keyword == "debit")
        {
            f.keyword = QString();
            f.flow = 1;
        }
        else if (f.keyword == "credit")
        {
            f.keyword = QString();
            f.flow = 2;
        }
        else
        {
            f.hasKeyword = true;
        }
        f.isEmpty = false;
    }

    return f;

}

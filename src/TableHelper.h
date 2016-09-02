#ifndef TABLE_HELPER_H
#define TABLE_HELPER_H

#include <QtCore>
#include <QTableWidget>

/**
 * @brief Custom table cell to enable column sorting with integers
 */
class IntegerTableItem : public QTableWidgetItem {
    public:
        IntegerTableItem(int value)
          : QTableWidgetItem(QString("%1").arg(value))
        {

        }

        bool operator <(const QTableWidgetItem &other) const
        {
            return text().toInt() < other.text().toInt();
        }
};

/**
 * @brief Base class helper
 */
class TableHelper : public QObject
{
    Q_OBJECT

public:
    TableHelper(QTableWidget *widget);

    void SetTableWidget(QTableWidget *widget) { mWidget = widget; }

    bool GetFirstColumnValue(int &value);
    void Initialize(const QStringList &header, int rows);
    void AppendLine(const QList<QVariant> &list, bool selected);
    void Finish();
    void SetSelectedColor(const QColor &color);
    void SetAlternateColors(bool enable);
    void Export(const QString &fileName);
private:
    QTableWidget *mWidget;
    QColor mSelectedColor;
    int mRow;
};

#endif // TABLE_HELPER_H

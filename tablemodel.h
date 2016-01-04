#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>
#include <QStringList>
#include <QTableView>
#include <QHeaderView>

class TableModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    explicit TableModel(QObject *parent = 0, bool _isMedia = true);
    ~TableModel(void);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    void setHorizontalHeaderList(QStringList horizontalHeaderList);
    void setVerticalHeaderList(QStringList verticalHeaderList);
    void setModalDatas(QList< QStringList > *rowlist);   
    void refrushModel();

//    bool insertRows(int row, int count, const QModelIndex &parent);
//    bool removeRows(int row, int count, const QModelIndex &parent);
//    bool setData(const QModelIndex &index, const QVariant &value, int role);

    void setMediaOrActor(bool _isMedia);
signals:

    void updateCount(int count);

private:

    QStringList horizontal_header_list;
    QStringList vertical_header_list;
    QList< QStringList > *arr_row_list;

    bool isMedia;
};


////
///
///
class TableView :public QTableView
{
    Q_OBJECT
public:
    explicit TableView(QWidget *parent = 0);
    ~TableView(void);
    void addRow(const QStringList rowList);
    int rowCount();

signals:

    void updateCount(int count);
public slots:
    void remove();
    void clear();
    void changevalue();
    void getValue(QList< QStringList > &data);

private:

private:
    TableModel *model;
    QList< QStringList > grid_data_list;
};



#endif // TABLEMODEL_H

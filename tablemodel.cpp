#include "tablemodel.h"
#include <QDebug>
#include <QPainter>
TableModel::TableModel(QObject *parent, bool _isMedia)
    :QAbstractTableModel(parent), arr_row_list(NULL)
{
    isMedia = _isMedia;
}

TableModel::~TableModel()
{
    arr_row_list = NULL;
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    if(vertical_header_list.size() > 0)
        return vertical_header_list.size();

    if(NULL == arr_row_list)
        return 0;
    else
        return arr_row_list->size();
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    if(horizontal_header_list.size() > 0)
        return horizontal_header_list.size();

    if(NULL == arr_row_list)
        return 0;
    else if(arr_row_list->size() < 1)
        return 0;
    else
        return arr_row_list->at(0).size();
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(NULL == arr_row_list)
        return QVariant();

    if(arr_row_list->size() < 1)
        return QVariant();

    if (role == Qt::TextAlignmentRole)
    {
        if(isMedia){
            if(index.column()  == 2 )
                return int(Qt::AlignLeft | Qt::AlignVCenter);
        }
        return int(Qt::AlignHCenter | Qt::AlignVCenter);
    }
    else if (role == Qt::DisplayRole)
    {
        if(index.row() >= arr_row_list->size())
            return QVariant();
        if(index.column() >= arr_row_list->at(0).size())
            return QVariant();
        return arr_row_list->at(index.row()).at(index.column());
    }
    return QVariant();
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
            return Qt::NoItemFlags;
    Qt::ItemFlags flag = QAbstractItemModel::flags(index);

//    flag|=Qt::ItemIsEditable // 设置单元格可编辑,此处注释,单元格无法被编辑
    return flag;
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role==Qt::DisplayRole)
    {
        if(orientation==Qt::Horizontal) // 水平表头
        {
            if(horizontal_header_list.size() > section)
                return horizontal_header_list[section];
            else
                return QVariant();
        }
        else
        {
            if(vertical_header_list.size() > section)
                return vertical_header_list[section]; // 垂直表头
            else
                return QVariant();
        }
    }

    return QVariant();
}

void TableModel::setHorizontalHeaderList(QStringList horizontalHeaderList)
{
    horizontal_header_list = horizontalHeaderList;
}

void TableModel::setVerticalHeaderList(QStringList verticalHeaderList)
{
    vertical_header_list = verticalHeaderList;
}

void TableModel::setModalDatas(QList<QStringList> *rowlist)
{    
    arr_row_list = rowlist;   
    if(!arr_row_list->isEmpty()){
    }
    refrushModel();
}


void TableModel::refrushModel()
{

    beginResetModel();
    endResetModel();
    emit updateCount(this->rowCount(QModelIndex()));
}

void TableModel::setMediaOrActor(bool _isMedia)
{
    isMedia = _isMedia;
}

//bool TableModel::insertRows(int row, int count, const QModelIndex &parent)
//{
//    beginInsertRows(QModelIndex(), row, row+count-1);
//    for (int row=0; row<count; row++){
//        arr_row_list->insert(row,  QStringList());
//    }
//    endInsertRows();
//    return true;
//}

//bool TableModel::removeRows(int row, int count, const QModelIndex &parent)
//{
//    beginRemoveRows(QModelIndex(), row, row+count-1);
//    for (int row=0; row<count; row++){
//        arr_row_list->removeAt(row);
//    }
//    endRemoveColumns();
//    return true;
//}


//bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
//{
//    if (index.isValid() && role == Qt::EditRole){
//        arr_row_list->at(index.row()).replace(index.column(), value.toString());
//        emit dataChanged(index.row(), index.column());
//        return true;
//    }

//    return false;
//}





/********************TableView********************/
TableView::TableView(QWidget *parent)
    : QTableView(parent)
{

    model = new TableModel();
    this->setModel(model);
    model->setModalDatas(&grid_data_list);
    connect(model, &TableModel::updateCount, this, &TableView::updateCount);
}

TableView::~TableView(void)
{
    if(model) {
        delete model;
        model = NULL;
    }
    grid_data_list.clear();
}

void TableView::addRow(const QStringList rowList)
{
    grid_data_list.append(rowList);
    model->refrushModel();
}

void TableView::remove()
{
    QModelIndexList model_index_list = this->selectedIndexes();
    int model_count = model_index_list.count();
    if(model_count <= 0)
        return;

    QList<int> list_row;
    for(int i=model_count-1; i>=0; i--)
    {
        QModelIndex model_index = model_index_list.at(i);
        int row = model_index.row();
        if(!list_row.contains(row))
            list_row.append(row);
    }

    if(list_row.isEmpty())
        return;

    qSort(list_row);

    for(int i=list_row.count()-1; i>=0; i--)
    {
        grid_data_list.removeAt(list_row.at(i));
    }

    model->refrushModel();
}

void TableView::clear()
{
    grid_data_list.clear();
    model->refrushModel();
}

void TableView::changevalue()
{
    //这里以10行尾列进行更新
    int row_count = this->rowCount();
    if(row_count < 10)
        return;

    for(int i=0; i<10; i++)
    {
        QStringList file_list = grid_data_list.at(i);
        int progress = rand()%100;
        QStringList row_list;
        row_list <<  file_list.at(0) << file_list.at(1) << QString::number(progress) << file_list.at(3) << file_list.at(4) << file_list.at(5);
        grid_data_list.replace(i, row_list);
    }

    model->refrushModel();
}

void TableView::getValue(QList<QStringList> &data)
{
    data = grid_data_list;
}

int TableView::rowCount()
{
    return model->rowCount(QModelIndex());
}



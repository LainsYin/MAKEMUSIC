#include "addsongdialog.h"
#include "ui_addsongdialog.h"
#include "tablemodel.h"
#include "yqcdelegate.h"
#include "mysqlquery.h"
#include "mainwindow.h"

#include <QSqlQuery>

AddSongDialog::AddSongDialog(QWidget *parent, MysqlQuery *sql) :
    QDialog(parent),
    ui(new Ui::AddSongDialog)
{
    ui->setupUi(this);
    initWidget();
    initTable();
    _sql = sql;
}

AddSongDialog::~AddSongDialog()
{
    delete ui;
}

void AddSongDialog::initWidget()
{
    ui->pushButton->setText("搜索");
    ui->lineEdit->setMinimumSize(180, 36);
}

void AddSongDialog::initTable()
{
    model = new TableModel(this);
    ui->widget_tableView->setModel(model);
    ui->widget_tableView->setAlternatingRowColors(true);

    ui->widget_tableView->setItemDelegate(new NoFocusDelegate());
    ui->widget_tableView->setSelectionMode(QAbstractItemView::NoSelection);
    ui->widget_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->widget_tableView->horizontalHeader()->setHighlightSections(false);
    ui->widget_tableView->verticalHeader()->setVisible(false);
    ui->widget_tableView->setShowGrid(false);
    ui->widget_tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->widget_tableView->verticalHeader()->setDefaultSectionSize(70);
    QHeaderView *headerView = ui->widget_tableView->horizontalHeader();
    //    headerView->setStretchLastSection(true);  ////最后一行适应空余部分
    headerView->setSectionResizeMode(QHeaderView::Stretch); //平均列宽

    ui->widget_tableView->show();
    ui->widget_tableView->setContextMenuPolicy(Qt::CustomContextMenu);

    QStringList headerList;
    headerList << "编号顺序" << "SERIAL_ID" << "MID" << "歌曲名" << "歌星名" << "语言" << "类型" << "" << "";

    ButtonDelegate *addDelegate = new ButtonDelegate(this);
    addDelegate->setButtonText("选择");
    ui->widget_tableView->setItemDelegateForColumn(8, addDelegate);

    connect(addDelegate, &ButtonDelegate::currentRow, this, &AddSongDialog::pushbuttonSelectSong);


    model->setHorizontalHeaderList(headerList);
    model->refrushModel();
    ui->widget_tableView->setColumnHidden(7, true);
    ui->widget_tableView->setColumnHidden(1, true);
    ui->widget_tableView->setColumnHidden(2, true);
}

void AddSongDialog::pushbuttonSelectSong(const int &row)
{
    Media media;
    QStringList rowValue = rowList.at(row);
    media.name = rowValue.at(3);
    media.serial_id = rowValue.at(1);
    media.mid = rowValue.at(2);
    media.singer = rowValue.at(4);
    media.vPath = rowValue.at(7);

    emit songName(media);
}

void AddSongDialog::on_pushButton_clicked()
{
    if(ui->lineEdit->text().isEmpty())
        return;

    QSqlQuery query;
    if(_sql->queryMedia(ui->lineEdit->text(), query))
    {
        int row = 0;
        QStringList rowValue;
        rowList.clear();
            while(query.next()){
                rowValue.clear();
                rowValue.append(QString::number(row+1));
                rowValue.append(query.value("_serial_id").toString());
                rowValue.append(query.value("_mid").toString());
                rowValue.append(query.value("_name").toString());
                rowValue.append(query.value("_singer").toString());
                rowValue.append(query.value("_language").toString());
                rowValue.append(query.value("_type").toString());
                rowValue.append(query.value("_path").toString());
                row++;
                rowList.append(rowValue);
            }

        model->setModalDatas(&rowList);
    }
}

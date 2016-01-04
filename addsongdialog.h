#ifndef ADDSONGDIALOG_H
#define ADDSONGDIALOG_H

#include <QDialog>

namespace Ui {
class AddSongDialog;
}

class Media;
class TableModel;
class MysqlQuery;
class AddSongDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddSongDialog(QWidget *parent = 0, MysqlQuery *sql = NULL);
    ~AddSongDialog();

private:
    void initWidget();
    void initTable();

private slots:
    void pushbuttonSelectSong(const int &row);

    void on_pushButton_clicked();

signals:
    void songName(const Media &_media);

private:
    Ui::AddSongDialog *ui;
    TableModel *model;

    QList< QStringList > rowList;
    MysqlQuery    *_sql;
};

#endif // ADDSONGDIALOG_H

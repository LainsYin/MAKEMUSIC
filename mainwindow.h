#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MysqlQuery;
class QPushButton;

struct Media{
    QString name;
    QString singer;
    QString serial_id;
    QString mid;
    QString vPath;
    bool    isMv; //true mv曲库， false mp3曲库
};

struct PointM{
    int     index; //节
    int     beat;  //节下分之分之几节拍
    int     curBeat; //当前节拍
    int     length_secs; //持续时长

    int     pos;
    QList<int> nums;    //拨动的哪些弦（多根弦）
    QList<int> time_secs;   //开始时间
    QList<QPair<int, int>> pnums;
};

enum Type{
    gt = 1,
    dr = 2,
    kb = 3
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void paintEvent(QPaintEvent *);
    void closeEvent(QCloseEvent *);
private slots:
    void on_lineEdit_bpm_editingFinished();

    void preview();

    void input_song_name(const Media &_media);

    void on_pushButton_guitar_clicked();

    void on_pushButton_drum_clicked();

    void on_pushButton_keyboard_clicked();

    void on_pushButton_input_clicked();

private:
    void initWidget();
    void readAndSetStyleSheet();
    bool isEmpyt();

private:
    Ui::MainWindow *ui;
    QPushButton *pushbutton_pre;

    MysqlQuery    *_sql;

//    QString singer;
    Media media;
};

#endif // MAINWINDOW_H

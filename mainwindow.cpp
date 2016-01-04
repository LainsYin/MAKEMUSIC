#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mysqlquery.h"
#include "addsongdialog.h"
#include "drumguitarkey.h"

#include <QFile>
#include <QDebug>
#include <QSettings>
#include <QPainter>
#include <QFileDialog>
#include <QStyleOption>
#include <QPushButton>
#include <QMessageBox>
#include <QProcessEnvironment>
#define DATABASE_NAME "yiqiding_ktv" //yiqiding_ktv
#define WIDGET_HEIGHT 36



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pushbutton_pre = new QPushButton(ui->lineEdit_musicName);
    readAndSetStyleSheet();
    initWidget();
    this->setWindowIcon(QIcon(":/Resources/img/logo.ico"));

    _sql = new MysqlQuery();
    _sql->openMysql(DATABASE_NAME);

    connect(pushbutton_pre, SIGNAL(clicked()), this, SLOT(preview()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void MainWindow::closeEvent(QCloseEvent *)
{

}

void MainWindow::initWidget()
{
    ui->label_bpm->setMinimumHeight(WIDGET_HEIGHT);
    ui->label_musicName->setMinimumHeight(WIDGET_HEIGHT);
    ui->label_notes->setMinimumHeight(WIDGET_HEIGHT);
    ui->label_serialid->setMinimumHeight(WIDGET_HEIGHT);

    ui->lineEdit_bpm->setMinimumHeight(WIDGET_HEIGHT);
    ui->lineEdit_musicName->setMinimumHeight(WIDGET_HEIGHT);
    ui->lineEdit_notes->setMinimumHeight(WIDGET_HEIGHT);
    ui->lineEdit_serialid->setMinimumHeight(WIDGET_HEIGHT);
    ui->lineEdit_offset->setMinimumHeight(WIDGET_HEIGHT);

    ui->pushButton_guitar->setMinimumSize(150, WIDGET_HEIGHT+10);
    ui->pushButton_drum->setMinimumSize(150, WIDGET_HEIGHT+10);
    ui->pushButton_keyboard->setMinimumSize(150, WIDGET_HEIGHT+10);

    ui->pushButton_input->setMinimumSize(180, 50);
    ui->pushButton_start->setMinimumSize(180, 50);
    pushbutton_pre->setMinimumSize(50, 26);
    pushbutton_pre->setText("浏览");
    QRect rect = ui->lineEdit_musicName->geometry();
    pushbutton_pre->setGeometry(rect.x()+5, rect.y()+5, 50, 26);

    ui->label_bpm->setText("BPM:");
    ui->label_musicName->setText("歌曲：");
    ui->label_notes->setText("NOTES:");
    ui->label_serialid->setText("SERIAL_ID:");
    ui->label_offset->setText("偏移量:");

    ui->lineEdit_offset->setPlaceholderText("单位毫秒");
    ui->pushButton_drum->setText("鼓");
    ui->pushButton_guitar->setText("吉他");
    ui->pushButton_keyboard->setText("键盘");
    ui->pushButton_start->setText("开始制作");
    ui->pushButton_input->setText("导入文件");


    // 设置输入框中文件输入区，不让输入的文字在被隐藏在按钮下
    ui->lineEdit_musicName->setTextMargins(60, rect.top(), rect.right(), rect.left());
    this->setWindowTitle("打谱工具");
}

void MainWindow::readAndSetStyleSheet()
{
    QFile qss(":/Resources/title.qss");
    qss.open(QFile::ReadOnly);
    this->setStyleSheet(qss.readAll());
    qss.close();

    ui->pushButton_guitar->setObjectName("Button");
    ui->pushButton_drum->setObjectName("Button");
    ui->pushButton_keyboard->setObjectName("Button");
    ui->pushButton_input->setObjectName("Button");
    ui->pushButton_start->setObjectName("Button");
    pushbutton_pre->setObjectName("Button");

    ui->lineEdit_bpm->setObjectName("LineEdit");
    ui->lineEdit_musicName->setObjectName("LineEdit");
    ui->lineEdit_notes->setObjectName("LineEdit");
    ui->lineEdit_serialid->setObjectName("LineEdit");

    ui->widget_center->setObjectName("CenterWidget");
}

bool MainWindow::isEmpyt()
{
    if(ui->lineEdit_bpm->text().isEmpty() ||
       ui->lineEdit_musicName->text().isEmpty() ||
       ui->lineEdit_notes->text().isEmpty() ||
       ui->lineEdit_serialid->text().isEmpty()){
        return true;
    } else {
        return false;
    }
}

void MainWindow::on_lineEdit_bpm_editingFinished()
{
    if( ui->lineEdit_bpm->text().toFloat() <= 0){

        QMessageBox::warning(this, "提示", QString("BPM输入错误\nBPM必须大于零！"));
    }
}

void MainWindow::preview()
{
//    AddSongDialog *song = new AddSongDialog(this, _sql);
//    connect(song, &AddSongDialog::songName, this, &MainWindow::input_song_name);
//    song->exec();
    QString fileFormat("(*.mp4)");
    QString desktop = QProcessEnvironment::systemEnvironment().value("USERPROFILE")+"\\Desktop";
    QString filePath = QFileDialog::getOpenFileName(this,
                                                 "添加文件",
                                                 desktop,
                                                 fileFormat
                                                 );

    if(!filePath.isEmpty()){
        QFile file(filePath);
        if(!file.exists())
            return;
        ui->lineEdit_musicName->setText(filePath);
    }
}

void MainWindow::input_song_name(const Media &_media)
{
    ui->lineEdit_musicName->setText(_media.name);
    ui->lineEdit_serialid->setText(_media.serial_id);

    media = _media;
}

void MainWindow::on_pushButton_guitar_clicked()
{
    DrumGuitarKey *guitar = new DrumGuitarKey(this,
                                              ui->lineEdit_musicName->text(),
                                              media.singer,
                                              ui->lineEdit_serialid->text(),
                                              ui->lineEdit_notes->text(),
                                              ui->lineEdit_musicName->text(),
                                              ui->lineEdit_offset->text().toInt(),
                                              ui->lineEdit_bpm->text().toInt(),
                                              Type::gt);
    guitar->exec();
}

void MainWindow::on_pushButton_drum_clicked()
{
    DrumGuitarKey *drum = new DrumGuitarKey(this,
                                              ui->lineEdit_musicName->text(),
                                              media.singer,
                                              ui->lineEdit_serialid->text(),
                                              ui->lineEdit_notes->text(),
                                              ui->lineEdit_musicName->text(),
                                              ui->lineEdit_offset->text().toInt(),
                                              ui->lineEdit_bpm->text().toInt(),
                                              Type::dr);
    drum->exec();
}

void MainWindow::on_pushButton_keyboard_clicked()
{
    DrumGuitarKey *key = new DrumGuitarKey(this,
                                              ui->lineEdit_musicName->text(),
                                              media.singer,
                                              ui->lineEdit_serialid->text(),
                                              ui->lineEdit_notes->text(),
                                              ui->lineEdit_musicName->text(),
                                              ui->lineEdit_offset->text().toInt(),
                                              ui->lineEdit_bpm->text().toInt(),
                                              Type::kb);
    key->exec();
}

void MainWindow::on_pushButton_input_clicked()
{
    QString fileFormat("(*.gdp)");
    QString desktop = QProcessEnvironment::systemEnvironment().value("USERPROFILE")+"\\Desktop";
    QString filePath = QFileDialog::getOpenFileName(this,
                                                 "添加文件",
                                                 desktop,
                                                 fileFormat
                                                 );

    if(!filePath.isEmpty()){
        QFile file(filePath);
        if(!file.exists())
            return;

        DrumGuitarKey *dgk = new DrumGuitarKey(this, filePath);
        dgk->exec();
    }
}

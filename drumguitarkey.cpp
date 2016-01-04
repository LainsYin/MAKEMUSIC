#include "drumguitarkey.h"
#include "ui_drumguitarkey.h"
#include "mainwindow.h"

#include <Common.h>
#include "Instance.h"
#include "Media.h"
#include "Enums.h"
#include "MediaPlayer.h"

#include <QPair>
#include <QFile>
#include <QPainter>
#include <QListView>
#include <QSettings>
#include <QKeyEvent>
#include <QDebug>
#include <QTime>
#include <QTextCodec>
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>
#include <QTextStream>
#include <QMessageBox>
#include <QtAlgorithms>
#include <QProcessEnvironment>

#define WIDGET_HEIGHT 36
#define CURRENT_INDEX 3

#define SHOW_NUM  5
#define SHOW_HEIGHT 50

#define RADIUS_DRUM 10
#define MAX_COUNT 2000


DrumGuitarKey::DrumGuitarKey(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DrumGuitarKey)
{
    ui->setupUi(this);
}

DrumGuitarKey::DrumGuitarKey(QWidget *parent, const QString &filePath) :
    QDialog(parent),
    ui(new Ui::DrumGuitarKey)
{
    ui->setupUi(this);
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this, "提示", "文件打开错误！");
    } else {

    QString str(file.readAll());
    QByteArray byte_array =  str.toLocal8Bit(); //json.toLocal8Bit();
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(byte_array, &json_error);
    if(json_error.error == QJsonParseError::NoError)
    {
        if(!parse_doucment.isObject())
            return;

        QJsonObject json = parse_doucment.object();
        if(json.contains("type")){
            QJsonValue type_value = json.take("type");
            QString typeStr = type_value.toString();
            if (typeStr.compare("guitar") == 0){
                _type = Type::gt;
            } else if (typeStr.compare("piano") == 0){
                _type = Type::kb;
            } else if (typeStr.compare("drum") == 0){
                _type = Type::dr;
            }
        }

        if(json.contains("bpm")){
            _bpm = json.take("bpm").toInt();
        }
        if(json.contains("offset")){
            _offset = json.take("offset").toInt();
        }

        if(json.contains("music")){
            _name = json.take("music").toString();
        }

        if(json.contains("singer")){
            _singer = json.take("singer").toString();
        }

        if(json.contains("serial_id")){
            _serial_id = json.take("serial_id").toString();
        }

        if(json.contains("vpath")){
            _vPath = json.take("vpath").toString();
        }

        if(json.contains("points"))
        {
            QJsonArray array = json["points"].toArray();
            for (int i=0; i<array.size(); i++)
            {
                PointM point;
                QJsonValue value = array.at(i);
                QJsonObject object = value.toObject();
                if (object.contains("index")){
                    point.index = object.take("index").toInt();
                }
                if (object.contains("beat")){
                    point.beat = object.take("beat").toInt();
                }
                if (object.contains("curBeat")){
                    point.curBeat = object.take("curBeat").toInt();
                }
//                if (object.contains("column")){
//                    point.column = object.take("column").toInt();
//                }
                if (object.contains("time_secs")){
//                    point.time_secs = object.take("time_secs").toInt();
                    QJsonArray arrTi = object["time_secs"].toArray();
                    foreach (QJsonValue value , arrTi) {
                        point.time_secs.append(value.toInt());
                    }
                }
                if (object.contains("length_secs")){
                    point.length_secs = object.take("length_secs").toInt();
                }
                if (object.contains("pnums")){
                   QJsonArray arrPn = object["pnums"].toArray();
                   bool first = true;
                   QPair<int, int> pair;
                   foreach(QJsonValue value , arrPn) {
                       if (first){
                           pair.first = value.toInt();
                           first = false;
                       } else {
                           pair.second = value.toInt();
                           first = true;
                           point.pnums.append(pair);
                       }
                   }
                }
                if (object.contains("nums")){
                    QJsonArray arrPn = object["nums"].toArray();
                    foreach (QJsonValue value , arrPn) {
                        point.nums.append(value.toInt());
                    }
                }

                points.append(point);
            }
        }

        initValue();
    }
    else {
        QMessageBox::warning(this, "提示", "JSON错误！");
    }
    file.close();
    }

}

DrumGuitarKey::DrumGuitarKey(QWidget *parent,
                             const QString &name,
                             const QString &singer,
                             const QString &serial_id,
                             const QString &notes,
                             const QString &vPath,
                             const int &offset,
                             const int &bpm,
                             const int &type) :
    QDialog(parent),
    _name(name), _singer(singer), _serial_id(serial_id), _notes(notes), _vPath(vPath), _offset(offset), _bpm(bpm), _type(type),
    ui(new Ui::DrumGuitarKey)
{
    ui->setupUi(this);

    points.clear();

    initValue();
}

void DrumGuitarKey::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QPainter *painter = new QPainter(this);
    ///绘制乐器
    drawInstrument(painter);

}

void DrumGuitarKey::drawDivision(QPainter *painter)
{
    QRect rect = ui->widget_rhythm->geometry();

    lines = getDivNum() * _count;
    int length = rect.width()/lines;
    for (int i=0; i<=lines; i++){
        QLine line1(rect.x() + i*length, rect.y() + rect.height()/2,
                    rect.x() + i*length, rect.y() + rect.height()/2 - SHOW_HEIGHT + 20);
        painter->drawLine(line1);
    }

    //节长度
    for (int j=0; j<=_count; j++){
        QLine line;
        line.setP1(QPoint(rect.x() + (j*getDivNum() - beatOfs) * length, rect.y() + rect.height()/2));
        line.setP2(QPoint(rect.x() + (j*getDivNum() - beatOfs) * length, rect.y() + rect.height()/2 - SHOW_HEIGHT));
        painter->drawLine(line);
        painter->drawText(line.p2(), QString::number(_startCount + j + 1));
    }

//    qDebug() << " cur ind " << _curInd
//             << " _cur count " << _curCount
//             << " start count " << _startCount
//             << " count " << _count
//             << " get num : " << getDivNum()
//             << " offset ; " << beatOfs;
    QPoint circleP(rect.x() + (_curInd) * length, rect.y() + rect.height()/2);
    painter->drawEllipse(circleP.x() - 10, circleP.y() - 20 - 4, 20, 20);
}

void DrumGuitarKey::drawInstrument(QPainter *painter)
{
    QRect rect = ui->widget_painter->geometry();
    drums.clear();
    XS.clear();
    YS.clear();
    if (_type == Type::dr){ //鼓

        QRect rectDR = ui->widget_rhythm->geometry();
        QLine line(rectDR.x() - 10, rectDR.y()+rectDR.height()/2,
                   rectDR.x()+rectDR.width() + 10, rectDR.y()+rectDR.height()/2);
        painter->setPen(QPen(QColor(0, 0, 0)));
        painter->drawLine(line);
        drawDivision(painter);

        ui->widget_rhythm->setHidden(false);
        int radius;
        int div = rect.height()/6;
        if (rect.width() < div * 2 * 4){
            radius = div/2;
        } else {
            radius = div;
        }

        ///相关的位置
        drums.insert(1, QRect(rect.x() +  rect.width()/2 - radius*4 - 10 - 5, rect.y() + div, div*2, div*2));
        drums.insert(2, QRect(rect.x() +  rect.width()/2 - radius*2 - 5, rect.y() + div, div*2, div*2));
        drums.insert(4, QRect(rect.x() +  rect.width()/2 + radius*2 + 10 + 5, rect.y() + div, div*2, div*2));
        drums.insert(3, QRect(rect.x() +  rect.width()/2 + 3, rect.y() + div, div*2, div*2));
        drums.insert(5, QRect(rect.x() +  rect.width()/2 - radius*3 - 10, rect.y() + div*3 - 10, div*2, div*2));
        drums.insert(6, QRect(rect.x() +  rect.width()/2 - radius, rect.y() + div*3 - 10, div*2, div*2));
        drums.insert(7, QRect(rect.x() +  rect.width()/2 + radius + 10,rect.y() + div*3 - 10, div*2, div*2));

        for(int i=0; i<drums.size(); i++){

            QRect rec = drums.value(i+1);
            painter->drawText(rec.x() - 5 + div, rec.y() - 5 + div, "鼓");
            painter->drawEllipse(rec);
        }
    } else if (_type == Type::kb){//键盘

        ui->widget_rhythm->setHidden(true);
        int div = rect.width()/2/9;
        int nums = getDivNum() * _count;
        int cell = rect.height()/nums; //每个小单元格的高度
        for (int i=0; i<8; i++){ ///画竖向的8基线
            painter->drawLine(rect.x() + 40 + div*i, rect.y() + rect.height(), rect.x() + 40 + div*i, rect.y());
            XS.insert(i, rect.x() + 40 + div*i);
        }

        for (int index = 0; index<=nums; index++){
            painter->drawLine(rect.x() + 40, rect.y() + rect.height() -  (cell*index),
                              rect.x() + div*7 + 40, rect.y() + rect.height() -  (cell*index));
            YS.insert(index * getDivNum(), rect.y() + rect.height() -  (cell*index));

            if ((index+beatOfs) % getDivNum() != 0){
                int textHeight = rect.x() - 10;
                drawText(painter, QPoint(textHeight, rect.y() + rect.height() - cell*(index)),
                         QString("%1/%2").arg(abs(index+beatOfs)%getDivNum()).arg(getDivNum()));
            }
         }

        //重绘节
        for (int j=0; j<=_count; j++){
            QLine line;
            line.setP1(QPoint(rect.x() + 40, rect.y() + rect.height() - (j*getDivNum() - beatOfs) * cell));
            line.setP2(QPoint(rect.x() + div*7 + 40, rect.y() + rect.height() - (j*getDivNum() - beatOfs) * cell));
            painter->save();
            painter->setPen(QPen(QBrush(QColor(255, 255, 0)), 2));
            painter->drawLine(line);
            painter->restore();
            QPoint textP = line.p1();
            textP.setX(textP.x() - 20);
            painter->save();
            painter->setPen(QPen(QBrush(QColor(255, 0, 0)), 2));
            painter->drawText(textP, QString::number(_startCount + j + 1));
            painter->restore();
        }
    } else if (_type == Type::gt){ //吉他

        ui->widget_rhythm->setHidden(true);
        int div = rect.height()/7;
        for (int i=5; i>=0; i--){
            painter->drawLine(rect.x() + 60, rect.y() + div*(i+1), rect.x() + rect.width() - 60, rect.y() + div*(i+1));
            YS.insert(5-i, rect.y() + div*(i+1));
        }

        int nums = getDivNum() * _count;
        int cell = (rect.width()-120)/nums; //每个小单元格的宽度
        for (int index = 0; index<=nums; index++){

            painter->drawLine(rect.x() + 60 + cell*(index), rect.y() + div,
                              rect.x() + 60 + cell*(index), rect.y() + rect.height() - div);
            XS.insert(index, rect.x() + 60 + cell*(index));

            if ((index+beatOfs) % getDivNum() != 0){
                int textHeight = rect.y() + rect.height() - div;
                drawText(painter, QPoint(rect.x() + 60 + cell*index - 15, textHeight),
                         QString("%1/%2").arg(abs(index+beatOfs)%getDivNum()).arg(getDivNum()));
            }
         }

        //重绘节
        painter->save();
        painter->setPen(QPen(QBrush(QColor(255, 255, 0)), 2));
        for (int j=0; j<=_count; j++){
            QLine line;
            line.setP1(QPoint(rect.x() + 60 + (j*getDivNum() - beatOfs) * cell, rect.y() + div));
            line.setP2(QPoint(rect.x() + 60 + (j*getDivNum() - beatOfs) * cell, rect.y() + rect.height() - div));
            painter->save();
            painter->setPen(QPen(QBrush(QColor(255, 255, 0)), 2));
            painter->drawLine(line);
            painter->restore();
            QPoint textP = line.p2();
            textP.setY(textP.y() + 20);
            painter->save();
            painter->setPen(QPen(QBrush(QColor(255, 0, 0)), 2));
            painter->drawText(textP, QString::number(_startCount + j + 1));
            painter->restore();
        }
        painter->restore();
    }

    if (points.size() > 0){

        QList<PointM> tpoints;
        tpoints.clear();
        selectPoint(tpoints);

        painter->save();
        painter->setPen(QPen(QBrush(QColor(0, 0, 0)), 2));
        painter->setBrush(QBrush(QColor(255, 0, 0)));
        foreach (PointM p, tpoints) {
            if (_type == Type::dr){
                int tempCB;
                if (p.curBeat >= 0)
                    tempCB = p.curBeat;
                else
                    tempCB = getDivNum() - 1 + abs(beatOfs);
                if(p.curBeat == tempCB && p.index == _startCount + _curCount){
                    foreach (int num, p.nums) {
                        painter->drawEllipse(drums[num]);
                    }
                }
            }else if (_type == Type::gt){
                foreach (int num, p.nums) {
                    int pos;
                    if (beatOfs < 0)
                        pos = p.curBeat - (getDivNum() + beatOfs);
                    else
                        pos = p.curBeat - beatOfs;
                    painter->drawRect(QRect(QPoint(XS.value((p.index - _startCount) * getDivNum() + pos),
                                                   YS.value(num+1)),
                                            QSize(XS.value(1) - XS.value(0), YS.value(0) - YS.value(1))));
                }
            } else if (_type == Type::kb){

                qDebug() << " size : " << tpoints.size() << "  " << beatOfs;
                int height = rect.height()/_count; //节的高度
                int cell = rect.height()/(getDivNum() * _count); //四分之一节拍每个节拍的高度
                int startP = rect.y() + rect.height() - (height*(p.index - _startCount));
                int _height16; //每个节拍十六分之一或十二分之一节拍高度
                int reNum = getDivNum();
                if (reNum == 16 || reNum == 8 || reNum == 4 || reNum == 2){
                    _height16 = height/16;
                } else if (reNum == 12 || reNum == 6 || reNum == 3){
                    _height16 = height/12;
                }

//                qDebug() << " painter index " << p.index
//                         << " painter curbeat " << p.curBeat
//                         << " painter nums " << p.nums
//                         << " painter beatofset" << beatOfs
//                         << " painter x : " << p.nums
//                         << " painter points : " << p.pos;
                foreach (int num, p.nums) {
                    painter->drawRect(QRect(QPoint(XS.value(num),
                                                   startP - _height16  * p.pos + (beatOfs>=0?cell*beatOfs:(getDivNum()+beatOfs)*cell)),
                                            QSize(XS.value(1) - XS.value(0), _height16)));
                }
//                for (int i=0; i<pm.nums.size(); i++){
//                    painter->drawRect(QRect(QPoint(XS.value(pm.column - 1), startP - _height * pm.nums.at(i)),
//                                            QSize(XS.value(1) - XS.value(0), _height)));
//                }

//                ///
//                for (int i=0; i<pm.pnums.size(); i++){
//                    //                            qDebug() << " column : " << pm.column << " size : " << pm.pnums.size();
//                    QPair<int, int> value  = pm.pnums.at(i);
//                    painter->drawRect(QRect(QPoint(XS.value(pm.column - 1), startP - _height * (value.second + value.first)),
//                                            QSize(XS.value(1) - XS.value(0), _height * (value.second + 1))));
//                }
            }
        }
    }
}

void DrumGuitarKey::selectPoint(QList<PointM> &tpoints)
{
    if(_type == Type::dr){
        int tep = 0;
        foreach (PointM pm, points){
            tep = beatOfs>=0?beatOfs:(getDivNum()+beatOfs);
            if(pm.curBeat == tep && pm.index == _startCount){
                tpoints.append(pm);
            }
        }
    } else {
        if (beatOfs == 0){
            foreach (PointM pm, points) {
                if (pm.index == _startCount + _curCount){
                    tpoints.append(pm);
                }
            }
        } else {
            foreach (PointM pm, points) {
                if (beatOfs < 0){
                    if((pm.index == _startCount + _curCount
                        &&(pm.curBeat >= getDivNum() - abs(beatOfs) && pm.curBeat < getDivNum()))
                            || (pm.index == _startCount + _curCount + 1)
                            &&  (pm.curBeat < getDivNum() - abs(beatOfs) && pm.curBeat >= 0  )){
                        tpoints.append(pm);
                    }
                } else if (beatOfs > 0){
                    if ((pm.index == _startCount + _curCount
                         && pm.curBeat >= beatOfs && pm.curBeat < getDivNum())
                            || (pm.index == _startCount + _curCount + 1
                                &&  pm.curBeat < beatOfs && pm.curBeat >= 0  )){
                        tpoints.append(pm);
                    }
                }
            }
        }
    }
}

bool DrumGuitarKey::delDouPoint(PointM &point)
{
    bool newP = true;
    for(int j=0; j<points.size();j++){
        PointM pp = points.at(j);
        if (_type == Type::kb){
            if ( pp.index == point.index
                 && pp.curBeat == point.curBeat
                 && pp.pos == point.pos
                 && pp.nums.indexOf(point.nums[0]) != -1){
                if (pp.nums.size() > 1){
                    pp.nums.removeOne(point.nums[0]);
                    points[j] = pp;
                }else{
                    points.removeAt(j);
                }
                newP = false;
            } else {
                if ( pp.index == point.index
                     && pp.curBeat == point.curBeat
                     && pp.pos == point.pos){
                    pp.nums.append(point.nums[0]);
                    points[j] = pp;
                    newP = false;
                }
            }
        } else {
            if( pp.index == point.index
                && pp.curBeat == point.curBeat
                && pp.nums.indexOf(point.nums[0]) != -1 ){
                if (pp.nums.size() > 1){
                    pp.nums.removeOne(point.nums[0]);
                    points[j] = pp;
                }else{
                    points.removeAt(j);
                }

                newP = false;
            } else {
                if (pp.index == point.index  && pp.curBeat == point.curBeat){
                    pp.nums.append(point.nums[0]);
                    points[j] = pp;
                    newP = false;
                }
            }
        }
    }

    return newP;
}

void DrumGuitarKey::drawText(QPainter *painter, const QPoint &sta, const QString &text)
{
    painter->save();
    painter->setPen(QPen(QBrush(QColor(0, 0, 0)), 2));
    painter->drawText(sta, text);
    painter->restore();
}

QList<QPoint> DrumGuitarKey::equDivide1(QPainter *painter, const QPoint &staP, const QPoint &endP, const QColor &color, const int &height)
{
    int div = (endP.x() - staP.x())/3;
    painter->save();
    painter->setPen(color);

    painter->drawLine(staP.x() + div*1, staP.y() - 1,
                      staP.x() + div*1, staP.y() - height);
    painter->drawLine(staP.x() + div*2, staP.y() - 1,
                      staP.x() + div*2, staP.y() - height);
    painter->restore();

    QPoint leftPoint(staP.x() + div*1, staP.y());
    QPoint rightPoint(staP.x() + div*2, staP.y());

    QList<QPoint> divs;
    divs.insert(0, leftPoint);
    divs.insert(1, rightPoint);

    return divs;
}


QPoint DrumGuitarKey::equDivide(QPainter *painter, const QPoint &staP, const QPoint &endP,
                                const QColor &color, const int    &height)
{
    painter->save();
    painter->setPen(color);
    painter->drawLine(staP.x() + (endP.x() - staP.x())/2, staP.y() - 1,
                      staP.x() + (endP.x() - staP.x())/2, staP.y() - height);
    painter->restore();

    return QPoint(staP.x() + (endP.x() - staP.x())/2, staP.y());
}

int DrumGuitarKey::getDivNum()
{
    int num = 1;
    QString text = ui->comboBox->currentText();
    if(text.compare("1/2") == 0)
        num = 2;
    else if (text.compare("1/3") == 0)
        num = 3;
    else if (text.compare("1/4") == 0)
        num = 4;
    else if (text.compare("1/6") == 0)
        num = 6;
    else if (text.compare("1/8") == 0)
        num = 8;
    else if (text.compare("1/9") == 0)
        num = 9;
    else if (text.compare("1/12") == 0)
        num = 12;
    else if (text.compare("1/16") == 0)
        num = 16;
    else
        num = 1;

    return num;
}

bool DrumGuitarKey::isInDrum(const QPoint &point, const QPoint &circle, const int &radius)
{
    if ( (((circle.x() - point.x()) * (circle.x() - point.x())) + ((circle.y() - point.y()) * (circle.y() - point.y())))
         > radius*radius)
         return false;
    return true;
}

void DrumGuitarKey::changePlayerStatue()
{
    if (_player->state() == Vlc::State::Playing)
        _player->pause();
    else if(_player->state() == Vlc::State::Paused)
        _player->play();
    else
        _player->play();
}

int DrumGuitarKey::isArea(const QPoint &point, const int &curbeat)
{
    QRect rect = ui->widget_painter->geometry();
    int height = rect.height()/_count;
    int div16 = height/16;
    int div12 = height/12;

    for (int i=0; i<_count; i++) {

        int startP;
        startP = rect.y() + rect.height() - (height*i);

        QList<int> list;
        switch (getDivNum()) {
        case 16: /// all
            list.append(2);
            list.append(4);
            list.append(6);
            list.append(8);
            list.append(10);
            list.append(12);
            list.append(14);
            list.append(16);
        case 8: /// 1/16 3/16 5/16 7/16 9/16 11/16 13/16 15/16
            list.append(3);
            list.append(7);
            list.append(11);
            list.append(15);
        case 4: /// 1/16  5/16  9/16  13/16
            list.append(5);
            list.append(13);
        case 2: ///1/16  9/16
            list.append(1);
            list.append(9);

            for (int i=0; i<list.size(); i++){
                if (point.y() < startP - ((list.at(i) - 1 )* div16)
                    && point.y() > startP - div16*(list.at(i)) ){

                    int num = list.at(i);
                    qDebug() << " offset : " << beatOfs << " num: " << num << " _curbeat " << curbeat;
                    if (beatOfs >= 0 && curbeat >= beatOfs){
                        num += getDivNum() * beatOfs;
                    } else if (beatOfs >=0 && curbeat < beatOfs) {
                        num -= getDivNum() * (getDivNum() - beatOfs);

                    } else if (beatOfs < 0 && curbeat >= getDivNum() + (beatOfs)){
                        num += getDivNum() * (getDivNum() - abs(beatOfs));
                    } else if (beatOfs < 0 && curbeat < getDivNum() + (beatOfs)){
                        num -= getDivNum() * abs(beatOfs);
                    }
                    return num<1?1:num;
                }
            }
            break;


        case 12: /// all
            list.append(2);
            list.append(4);
            list.append(6);
            list.append(8);
            list.append(10);
            list.append(12);
        case 6: /// 1/12  3/12  5/12  7/12 9/12  11/12
            list.append(3);
            list.append(7);
            list.append(11);
        case 3: /// 1/12  5/12  9/12
            list.append(1);
            list.append(5);
            list.append(9);
            foreach (int num, list) {

                if (point.y() < startP + ((num - 1 )* div12)
                    && point.y() > startP + div12*num ){

                    if (beatOfs >= 0 && curbeat > beatOfs){
                        num += getDivNum() * beatOfs;
                    } else if (beatOfs >=0 && curbeat <= beatOfs) {
                        num -= getDivNum() * (getDivNum() - beatOfs);
                    } else if (beatOfs < 0 && curbeat > abs(beatOfs)){
                        num += getDivNum() * (getDivNum() - abs(beatOfs));
                    } else if (beatOfs < 0 && curbeat <= abs(beatOfs)){
                        num -= getDivNum() * abs(beatOfs);
                    }
                    return num<1?1:num;
                }
            }
            break;
        default:
            break;
        }
    }

    return -1;
}

bool DrumGuitarKey::drawSingle(QPainter *painter, const int &index,
                               const int &curbeat, const int &first, const int &second)
{
    QRect rect = ui->widget_painter->geometry();
    int height = rect.height()/_count;
    int div16 = height/16;
    int div12 = height/12;

    int num = first;
//    for (int i=_startCount; i <= index; i++) {

        int startP;
        startP = rect.y() + rect.height() - (height*(index - 1));

        int reNum = getDivNum();
        if (reNum == 16 || reNum == 8 || reNum == 4 || reNum == 2){
            qDebug() << " XS values ;" << XS.values();
            qDebug() << " XS num ;" << num << " curBeat : " << curbeat << " point size : " << points.size();
            painter->drawRect(QRect(QPoint(XS.value(num - 1),
                                           startP - div16 * second),
                                    QSize(XS.value(1) - XS.value(0), div16)));


        } else if (reNum == 12 || reNum == 6 || reNum == 3){

        }
//        point.y() < startP - ((list.at(i) - 1 )* div16)
//                            && point.y() > startP - div16*(list.at(i))


//    }

        return -1;
}

void DrumGuitarKey::closeEvent(QCloseEvent *)
{
    if(_player)
        _player->stop();

    on_pushButton_ret_clicked();
}

void DrumGuitarKey::keyPressEvent(QKeyEvent *key)
{
    key->ignore();
}

void DrumGuitarKey::mousePressEvent(QMouseEvent *mouse)
{
    // 获取鼠标在点击窗体上的坐标
    QPoint pos = mouse->pos();
    if ( mouse->button() == Qt::LeftButton
         && QApplication::keyboardModifiers () == Qt::ControlModifier
         && _type == Type::kb)
    {
        ctrl = true;
        int sigP = isArea(pos, beatOfs);
        if (sigP == -1) {
            return;
        }

        int column = -1;
        QList<int> xkeys = XS.keys();
        QList<int> yKeys = YS.keys();
        for (int i=0; i<xkeys.size() - 1; i++) {
            if (pos.x() > XS.value(xkeys.at(i)) && pos.x() < XS.value(xkeys.at(i+1))){
                column = i+1;
                break;
            }
        }

        if(!startP){
            startP = true;
            for (int i=0; i<yKeys.size() - 1; i++){
                if(pos.y() < YS.value(yKeys.at(i))
                        && pos.y() > YS.value(yKeys.at(i+1))){
                    _curCount = (i) / (getDivNum());
                    break;
                }
            }
            multiP.curBeat = isArea(pos, beatOfs); //作开始点记录
//            multiP.column = column;
            multiP.index = _curCount  + _startCount;
            multiP.beat = getDivNum();

        } else {
            startP = false;
            PointM point; ///行列 x+1 y+1
            point.nums.clear();
            point.pnums.clear();
            point.index = _curCount  + _startCount;
            point.beat = getDivNum();
//            point.column = column;
            point.curBeat = isArea(pos, beatOfs);
            int single = isArea(pos, beatOfs);
            if (single == -1){
                return;
            }

//            qDebug() << " newP : " << point.column << " start column : " << multiP.column;
            ///判断是否已经存在 //存在同节删除
            for (int j = 0; j < points.size(); j++){
                PointM pp = points.at(j);
                if (pp.pnums.size() > 0){
                    QPair<int, int> pair = pp.pnums.first();
//                    if ( pp.index == point.index && pp.column == point.column
//                         && pair.first == multiP.curBeat && pair.second == single - multiP.curBeat){
//                        points.removeAt(j);
//                        this->repaint();
//                        return;
                    }
                }
            }

            int count = 0;
            int reNum = getDivNum();
            int ofs;
//            ofs = (_timeUnit/getDivNum()) * (multiP.column % getDivNum());
            if (reNum == 16 || reNum == 8 || reNum == 4 || reNum == 2){
                count = 16;
                ofs = _timeUnit / 16;
            } else if (reNum == 12 || reNum == 6 || reNum == 3){
                count = 12;
                ofs = _timeUnit / 12;
            }

//            QPair<int, int> _pair(multiP.curBeat, (multiP.index - point.index) * count + single - multiP.curBeat);
//            point.time_secs[0] = (_startCount + _curCount) * _timeUnit + ofs * (_pair.first - 1) ;
//            point.length_secs = ofs * (_pair.first + _pair.second);
//            point.pnums.append(_pair);
//            points.append(point);
            this->repaint();
//        }

    }
    mouse->ignore();
}

void DrumGuitarKey::mouseReleaseEvent(QMouseEvent *mouse)
{
    QPoint mouPo = mouse->pos();
    if( mouPo.x() < ui->widget_painter->x() + 10
        || mouPo.x() > (ui->widget_painter->x() + ui->widget_painter->width() - 10)
        || mouPo.y() < ui->widget_painter->y() + 10
        || mouPo.y() > (ui->widget_painter->y() + ui->widget_painter->height() - 10)){
        return;
    }

    switch (_type) {
    case Type::dr: //鼓
    {
        QList<int> listD = drums.keys();
        for(int i=0; i<listD.size(); i++) {

            QRect rectDR = drums.value(listD.at(i));
            if(isInDrum(mouPo, QPoint(rectDR.x() + rectDR.width()/2, rectDR.y() + rectDR.height()/2), rectDR.width()/2)){

                _curCount = (_curInd) / (getDivNum()); ///i error
                PointM point;
                point.index = _curCount  + _startCount;
                point.beat = getDivNum();
                point.curBeat = beatOfs>=0?beatOfs:getDivNum()+beatOfs;
                point.nums.clear();
                point.pnums.clear();
                point.nums.append(listD.at(i));
                ///判断是否已经存在 //存在同节删除
                if(delDouPoint(point)){

                    points.append(point);
//                    int ofs = (_timeUnit/getDivNum()) * ((i + 1) % getDivNum());
//                    point.time_secs.append((_startCount + _curCount) * _timeUnit + ofs);
//                    point.length_secs = 0; //ofs;
//                    point.column = -1;
//                    point.point = QPoint(rectDR.x() + rectDR.width()/2, rectDR.y() + rectDR.height()/2);
                }
                this->repaint();
            }
        }
        break;
    }
    case Type::kb: ///键盘
    {
        if ( mouse->button() != Qt::LeftButton
             || QApplication::keyboardModifiers() == Qt::ControlModifier)
            return;
        ctrl = false;
        startP = false;

        int x = -1, y = -1;
        QList<int> xkeys = XS.keys();
        QList<int> yKeys = YS.keys();
        if ( mouPo.x() < XS.first() || mouPo.x() > XS.last()
             || mouPo.y() < YS.last() || mouPo.y() > YS.first()){
            break;
        }

        for (int i=0; i<xkeys.size() - 1; i++) {
            if (mouPo.x() > XS.value(xkeys.at(i)) && mouPo.x() < XS.value(xkeys.at(i+1))){
                x = i;
                break;
            }
        }
        ///
        for (int i=0; i<yKeys.size() - 1; i++){
            if(mouPo.y() < YS.value(yKeys.at(i))
                    && mouPo.y() > YS.value(yKeys.at(i+1))){
                y = i;
                _curCount = (i) / (getDivNum());
                break;
            }
        }


        if ( x != -1 && y != -1 && isArea(mouPo, beatOfs) != -1){
            PointM point; ///行列 x+1 y+1
            point.index = (beatOfs>=0?abs(y+beatOfs)/getDivNum():(y+getDivNum()+beatOfs)/getDivNum())  + _startCount;
            point.beat = getDivNum();
            point.nums.clear();
            point.pnums.clear();
            point.curBeat = beatOfs >= 0?((y + beatOfs) % getDivNum()):((getDivNum()+beatOfs + y) % getDivNum());
            point.pos = isArea(mouPo, point.curBeat);
            point.nums.append(x);

            qDebug() << " index " << point.index
                     << " curbeat " << point.curBeat
                     << " nums " << point.nums
                     << " beatofset" << beatOfs
                     << " x : " << x
                     << " points : " << point.pos
                     << " _curcount : " << _curCount
                     << " _startcount : " << _startCount;

            ///判断是否已经存在 //存在同节删除
            if(delDouPoint(point)){
//                int ofs;
//                ofs = (_timeUnit/getDivNum()) * ((x + 1) % getDivNum());
//                point.time_secs.append((_startCount + _curCount) * _timeUnit + ofs);
//                point.length_secs = 0;
                points.append(point);
            }
            this->repaint();

        }
        break;
    }
    case Type::gt: ///吉他
    {
        ///
        int x = -1, y = -1;
        QList<int> xkeys = XS.keys();
        QList<int> yKeys = YS.keys();
        if ( mouPo.x() < XS.first() || mouPo.x() > XS.last()
             || mouPo.y() < YS.last() || mouPo.y() > YS.first()){
            break;
        }

        for (int i=0; i<xkeys.size() - 1; i++) {
            if (mouPo.x() > XS.value(xkeys.at(i)) && mouPo.x() < XS.value(xkeys.at(i+1))){
                x = i;
                _curCount = (i) / (getDivNum());
                break;
            }
        }

        for (int i=0; i<yKeys.size() - 1; i++){
            if(mouPo.y() < YS.value(yKeys.at(i)) && mouPo.y() > YS.value(yKeys.at(i+1))){
                y = i;
                break;
            }
        }

        if ( x != -1 && y != -1){

            PointM point; ///行列 x+1 y+1
            point.index = (beatOfs>=0?abs(x+beatOfs)/getDivNum():(x+getDivNum()+beatOfs)/getDivNum())  + _startCount;
            point.beat = getDivNum();
            point.nums.clear();
            point.pnums.clear();
            point.nums.append(y);
            point.curBeat = beatOfs>=0?((x + beatOfs) % getDivNum()):((getDivNum()+beatOfs+x) % getDivNum());
            qDebug() << " index " << point.index
                     << " curbeat " << point.curBeat
                     << " nums " << point.nums
                     << " beatofset" << beatOfs
                     << " y : " << y << "  " << "x : " << x << _curCount << "  " << _startCount;
            ///判断是否已经存在 //存在同节删除
            if(delDouPoint(point)){
                int ofs;
                points.append(point);
//                ofs = (_timeUnit/getDivNum()) * ((y + 1) % getDivNum());
//                point.time_secs.append((_startCount + _curCount) * _timeUnit + ofs);
//                point.length_secs = ofs;
//                point.column = -1;
            }
            this->repaint();
        }

        break;
    }
    default:
        break;
    }

    mouse->ignore();
}

void DrumGuitarKey::mouseMoveEvent(QMouseEvent *mouse)
{
    mouse->ignore();
}

void DrumGuitarKey::wheelEvent(QWheelEvent *wheel)
{
    int numDegrees = wheel->delta() / 8;
    int numSteps = numDegrees / 15;

    if((_startCount < 1 && numSteps < 0 && _curInd < 1)
        || (_startCount >= _maxCount && numSteps > 0 )){
        wheel->ignore();
    } else {
        if (numSteps < 0){
            if(_type == Type::dr){
                if (_curInd > 0){
                    _curInd--;
                    _curCount = _curInd  / getDivNum();
                }
                if (_curInd == 0 && beatOfs > 0 && abs(beatOfs) != getDivNum()){
                    beatOfs--;
                }
                if (_curInd == 0 && _startCount > 0){
                    _curCount = 0;

                    beatOfs--;
                    if (abs(beatOfs) == getDivNum()){
                        _startCount--;
                        beatOfs = 0;
                    }
                }
            } else {
                if (_startCount >= 0){
                    if (_startCount == 0 && _curCount == 0 && beatOfs ==0)
                        return;
                    beatOfs--;
                if (abs(beatOfs) == getDivNum()){
                    if (_startCount > 0)
                        _startCount--;
                    beatOfs = 0;
                }
                }
            }
//            if(_player->state() == Vlc::State::Opening)
//                ui->widget_seek->updateTime(-_timeUnit);
        } else {
            if(_type == Type::dr){
                if(_curInd <  getDivNum() * _count ){
                    _curInd++;
                    _curCount = _curInd / getDivNum();
                }
                if(_curInd == (_count * getDivNum()) && (_count + _startCount) != _maxCount){
                    _curCount = _curInd / getDivNum();
                    beatOfs++;
                    if (isFirstOfset){
                        beatOfs = 0;
                        isFirstOfset = false;
                    }
                    if (beatOfs >= getDivNum()){
                        _startCount++;
                        beatOfs = 0;
                    }
                }
            } else {
                beatOfs++;
                if (beatOfs >= getDivNum()){
                    _startCount++;
                    beatOfs = 0;
                }
            }
//            if(_player->state() == Vlc::State::Opening)
//                ui->widget_seek->updateTime(_timeUnit);
        }
        this->repaint();

        if(!_vPath.isEmpty()){
            ui->widget_seek->wheelUpdateTime(_startCount * _timeUnit);
        }
    }
}

bool DrumGuitarKey::eventFilter(QObject *dialog, QEvent *event)
{
    int indexU, indexD;
    if(event->type() == QEvent::KeyPress){
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        switch (keyEvent->key()) {
        case Qt::Key_Space:
            if (ui->pushButton_pause->isChecked()){
                ui->pushButton_pause->setChecked(false);
                changePlayerStatue();
            } else {
                ui->pushButton_pause->setChecked(true);
                changePlayerStatue();
            }
            break;
        case Qt::Key_Left:
//            if(_type == Type::dr){
//                if (_curInd > 0){
//                    _curInd--;
//                    _curCount = _curInd  / getDivNum();
//                }
//                if (_curInd == 0 && beatOfs > 0 && abs(beatOfs) != getDivNum()){
//                    beatOfs--;
//                }
//                if (_curInd == 0 && _startCount > 0){
//                    _curCount = 0;

//                    beatOfs--;
//                    if (abs(beatOfs) == getDivNum()){
//                        _startCount--;
//                        beatOfs = 0;
//                    }
//                }
//            } else {
            if (_startCount >= 0){
                if (_startCount == 0 && _curCount == 0 && beatOfs ==0)
                    break;
                beatOfs--;

                if (beatOfs >= 0){
                if (beatOfs == getDivNum()){
                    if (_startCount > 0)
                        _startCount--;
                    beatOfs = 0;
                }
                }
                if (beatOfs < 0){
                    if (getDivNum() - 1 + abs(beatOfs) == getDivNum()){
                        if (_startCount > 0)
                            _startCount--;
                    }
                    if (abs(beatOfs) == getDivNum()){
                        beatOfs = 0;
                    }
                }


                qDebug() << " start " << _startCount;
            }
//            }
//            if(_player->state() == Vlc::State::Opening)
//                ui->widget_seek->updateTime(-_timeUnit);
            break;
        case Qt::Key_Right:
//            if(_type == Type::dr){
//                if(_curInd <  getDivNum() * _count ){
//                    _curInd++;
//                    _curCount = _curInd / getDivNum();
//                }
//                if(_curInd == (_count * getDivNum()) && (_count + _startCount) != _maxCount){
//                    _curCount = _curInd / getDivNum();
//                    beatOfs++;
//                    if (isFirstOfset){
//                        beatOfs = 0;
//                        isFirstOfset = false;
//                    }
//                    if (beatOfs >= getDivNum()){
//                        _startCount++;
//                        beatOfs = 0;
//                    }
//                }
//            } else {
                beatOfs++;
                if (beatOfs >= getDivNum()){
                    _startCount++;
                    beatOfs = 0;
                }
//            }

//            if(_player->state() == Vlc::State::Opening)
//                ui->widget_seek->updateTime(_timeUnit);
            break;
        case Qt::Key_Up:
            indexU = ui->comboBox->currentIndex();
            if (indexU > 0)
                indexU--;
            ui->comboBox->setCurrentIndex(indexU);
            break;
        case Qt::Key_Down:
        {
            indexD = ui->comboBox->currentIndex();
            if (indexD < ui->comboBox->count() - 1)
                indexD++;
            ui->comboBox->setCurrentIndex(indexD);
            break;
        }
        default:
            break;
        }

        this->repaint();
        return true;
    }
    return QDialog::eventFilter(dialog, event);
}

DrumGuitarKey::~DrumGuitarKey()
{
    delete ui;
}

void DrumGuitarKey::initWidget()
{
    ui->label_bpm->setMinimumHeight(WIDGET_HEIGHT);
    ui->label_name->setMinimumHeight(WIDGET_HEIGHT);
    ui->label_notes->setMinimumHeight(WIDGET_HEIGHT);
    ui->label_serial_id->setMinimumHeight(WIDGET_HEIGHT);

    ui->pushButton_ret->setMinimumSize(100, WIDGET_HEIGHT+10);
    ui->pushButton_save->setMinimumSize(100, WIDGET_HEIGHT+10);

    ui->comboBox->setMinimumSize(160, WIDGET_HEIGHT);

    ui->label_bpm->setText(QString("BMP : %1").arg(_bpm));
    ui->label_name->setText(QString("%1  %2").arg(_name).arg(_singer));
    ui->label_notes->setText(QString("NOTES : %1").arg(_notes));
    ui->label_serial_id->setText(QString("SERIAL_ID : %1").arg(_serial_id));

    ui->pushButton_ret->setText("返回");
    ui->pushButton_save->setText("保存");
    ui->pushButton_pause->setCheckable(true);

    items.clear();
    items << "1" << "1/2" << "1/3" << "1/4" << "1/6" << "1/8" << "1/9" << "1/12" << "1/16" << "free";
    ui->comboBox->addItems(items);
    ui->comboBox->setCurrentIndex(CURRENT_INDEX);

    QString title;
    switch (_type) {
    case Type::gt:
        title = "吉他";
        break;
    case Type::dr:
        title = "鼓";
        break;
    case Type::kb:
        title = "键盘";
        break;
    default:
        break;
    }
    this->setWindowTitle(title);
    //添加最大化和最小化窗口 关闭窗口不能动
    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowMinMaxButtonsHint;
    flags |= Qt::WindowMaximizeButtonHint;
    flags |= Qt::WindowCloseButtonHint;
    this->setWindowFlags(flags);
}

void DrumGuitarKey::readAndSetStyleSheet()
{
    QFile qss(":/Resources/title.qss");
    qss.open(QFile::ReadOnly);
    this->setStyleSheet(qss.readAll());
    qss.close();

    ui->pushButton_ret->setObjectName("Button");
    ui->pushButton_save->setObjectName("Button");
    ui->pushButton_pause->setObjectName("Play");

    ui->pushButton_plus->setObjectName("PlusButton");
    ui->pushButton_min->setObjectName("MinButton");
    ui->comboBox->setObjectName("combobox");

    QListView *view = new QListView();
    ui->comboBox->setView(view);
    ui->comboBox->setMaxVisibleItems(5);
}

void DrumGuitarKey::initValue()
{
    initWidget();
    readAndSetStyleSheet();
    timer = new QTimer(this);
    _count = 1;
    _curCount = 0;
    _startCount = 0;
    _curInd = 0;
    ctrl = true;
    startP = false;
    lines = 0;
    beatOfs = 0;
    _maxCount = MAX_COUNT;
    isFirstOfset = true;

//    _instance = new VlcInstance(VlcCommon::args(), this);
//    _player = new VlcMediaPlayer(_instance);
//    _player->setVideoWidget(ui->widget_video);
/*//    ui->widget_mp4Video->setMediaPlayer(_player);
//    ui->widget_mp4Volume->setMediaPlayer(_player);
//    ui->widget_mp4Volume->setVolume(50);
*/
//    ui->widget_video->setHidden(true);
//    ui->widget_seek->setMediaPlayer(_player);

    ui->widget_video->setHidden(true);
    ui->pushButton_pause->setFocus(); //设置默认焦点
    ui->pushButton_pause->setShortcut(QKeySequence::InsertParagraphSeparator); ///设置快捷键为键盘的回车键
    ui->pushButton_pause->setShortcut(Qt::Key_Pause);//键盘的pause键

    ui->pushButton_pause->installEventFilter(this);
    ui->comboBox->installEventFilter(this);
    ui->widget_rhythm->installEventFilter(this);


    if (_type == Type::gt)
        typeStr = "guitar";
    else if(_type == Type::kb)
        typeStr = "piano";
    else if (_type == Type::dr)
        typeStr = "drum";
    else
        typeStr = "other";

    _json.insert("type", typeStr);
    _json.insert("music", _name);
    _json.insert("singer", _singer);
    _json.insert("bpm",  QJsonValue(_bpm));
    _json.insert("offset", QJsonValue(_offset));
    _json.insert("serial_id", QJsonValue(_serial_id));
    _json.insert("vpath", QJsonValue(_vPath));
    _timeUnit = (((240*1.0)/_bpm) * 1000);
/*
//    resUrl = new QSettings("config.ini", QSettings::IniFormat);
//    resUrl->setIniCodec("UTF-8");
//    _resUrl = resUrl->value("SERVER/res").toString();
//    _resUrl.append(_vPath);*/
    _resUrl = _vPath;
    qDebug() << " url : " << _resUrl;
//    _media = new VlcMedia(_resUrl, true, _instance);
//    if(!_vPath.isEmpty()){
//        _player->open(_media); ///openOnly()
//        connect(timer, &QTimer::timeout, this, &DrumGuitarKey::firstUpdateTime);
//        connect(ui->widget_seek, &VlcWidgetSeek::jumpUpdateTime, this, &DrumGuitarKey::jumpUpdateTime);
//        connect(_player, &VlcMediaPlayer::end, this, &DrumGuitarKey::playEnd);
//        timer->start(120);
//    }

}

void DrumGuitarKey::on_pushButton_pause_clicked(bool checked)
{
    if(checked){
//        qDebug() <<  "  checked :  " << checked << " 运行";
//        _player->pause();

    } else {
//       qDebug() <<  "  checked :  " << checked << " 暂停";
//        if(_player->state() != Vlc::State::Paused){
//        } else {

//       _player->play();
//        }
    }

    changePlayerStatue();
}

void DrumGuitarKey::on_comboBox_currentIndexChanged(int)
{
    this->repaint();
}

void DrumGuitarKey::on_pushButton_plus_clicked()
{
    if(_count <= 10)
        _count++;
    this->repaint();
}

void DrumGuitarKey::on_pushButton_min_clicked()
{
    if(_count > 1 )
        _count--;
    this->repaint();
}

void DrumGuitarKey::on_pushButton_ret_clicked()
{
    if (QMessageBox::warning(this, "保存", "是否保存数据？",
                             QMessageBox::Ok,
                             QMessageBox::Cancel) == QMessageBox::Ok){
        on_pushButton_save_clicked();
    }
}

bool DrumGuitarKey::orderPoints()
{
    ///按开始时间排序
    for(int i = 0; i<points.size(); i++){

        PointM point = points.at(i);
        if (point.nums.size() > 1){
            qSort(point.nums.begin(), point.nums.end());
        }
        if (point.time_secs.size() > 1){
            qSort(point.time_secs.begin(), point.time_secs.end());
        }

        for (int j = i+1; j < points.size(); j++){
            //排序条件:按长度从小到大
            if(points.at(i).time_secs[0] > points.at(j).time_secs[0])
            {
                points.swap(i,j);
            }
        }
    }
}


void DrumGuitarKey::addPoints(TimePoint point, QList<TimePoint> &times)
{
    for(int i=0; i<times.size(); i++) {
        TimePoint tp = times.at(i);
        if (tp.time == point.time  && tp.length == tp.length)
        {
            tp.ids.append(point.ids);
            times[i] = tp;
            return;
        }
    }

    times.append(point);
}


QByteArray DrumGuitarKey::writeAppFile()
{
    QList<TimePoint> times;
    times.clear();
    foreach (PointM point, points) {
        TimePoint tt;
        if (point.time_secs.size() > 1){
            foreach (int index, point.time_secs) {
                tt.length = point.length_secs;
                tt.time = point.time_secs[index];
                tt.ids = point.nums;
                addPoints(tt, times);
           }

        } else {
            tt.length = point.length_secs;
            tt.time = point.time_secs[0];
            tt.ids = point.nums;
            addPoints(tt, times);
        }
    }

    QJsonArray array;
    QJsonObject tjson;
    tjson.insert("type", typeStr);
    tjson.insert("music", _name);
    tjson.insert("singer", _singer);
    tjson.insert("bpm",  QJsonValue(_bpm));
    tjson.insert("serial_id", QJsonValue(_serial_id));
    foreach (TimePoint point, times) {

        QJsonObject json;
        QJsonArray ids;
        foreach (int id, point.ids) {
            ids.append(QJsonValue(id));
        }
        json.insert("ids", QJsonValue(ids));
        json.insert("length", QJsonValue(point.length));
        json.insert("time", QJsonValue(point.time));

        array.append(QJsonValue(json));
    }
    tjson.insert("points", QJsonValue(array));

    QJsonDocument document;
    document.setObject(tjson);
    QByteArray byteArray = document.toJson(QJsonDocument::Indented);  ///Indented Compact

    return byteArray;
}


void DrumGuitarKey::on_pushButton_save_clicked()
{
    QString filePath;
    QString fileFormat("(*.gdp)");
    QString desktop = QProcessEnvironment::systemEnvironment().value("USERPROFILE")+"\\Desktop";
    filePath = QFileDialog::getSaveFileName(this,
                                            QString("保存文件"),
                                            desktop,
                                            fileFormat);


    orderPoints();

    qDebug() << " ****************  ";
    QJsonArray array;
    foreach (PointM point, points) {

        QJsonObject json;
        json.insert("index", QJsonValue(point.index));
        json.insert("beat", QJsonValue(point.beat));
        json.insert("curBeat", QJsonValue(point.curBeat));
//        json.insert("column", QJsonValue(point.column));
        json.insert("length_secs", QJsonValue(point.length_secs));

        QJsonArray times;
        foreach (int time, point.time_secs) {
            times.append(QJsonValue(time));
        }
        json.insert("time_secs", QJsonValue(times));

        QJsonArray nums;
        foreach (int num, point.nums) {
            nums.append(QJsonValue(num));
        }
        QJsonArray pnums;
        QList<QPair<int, int>> pps = point.pnums;
        for (int i=0; i<point.pnums.size(); i++){

            QPair<int, int> pnum = point.pnums.at(i);
            pnums.append(QJsonValue(pnum.first));
            pnums.append(QJsonValue(pnum.second));
        }

        if(pnums.size() > 0)
            json.insert("pnums", QJsonValue(pnums));
        if (nums.size() > 0)
            json.insert("nums", QJsonValue(nums));

        array.append(QJsonValue(json));
    }

    _json.insert("points", QJsonValue(array));


    QJsonDocument document;
    document.setObject(_json);

    ///local tool
    QString localPath = filePath;
    QByteArray byteArray = document.toJson(QJsonDocument::Indented);  ///Indented Compact
    QFile file(localPath.append(".swp"));
    QTextStream out(&file);
    if(file.exists())
        file.remove();
    if(file.open(QIODevice::Append | QIODevice::WriteOnly))
    {
        out << byteArray;
        file.close();
    }



    ///app
    QFile appfile(filePath);
    QTextStream appout(&appfile);
    if(appfile.exists())
        appfile.remove();
    if(appfile.open(QIODevice::Append | QIODevice::WriteOnly))
    {
        appout << writeAppFile();
        appfile.close();
    }
}

void DrumGuitarKey::firstUpdateTime()
{
    timer->stop();
    QString secStrTemp = ui->widget_seek->returnSeekTime();
    QStringList list = secStrTemp.split("_&&_");
    QTime fulltime = QTime::fromString(list.last(), "mm:ss");
    int second = fulltime.minute() * 60 + fulltime.second();
    if(second != 0){
        _maxCount = ((second*1000)/_timeUnit);
        _maxCount += ((second*1000)%_timeUnit > 0)?1:0;

    }

    _json.insert("second", QJsonValue(second));
    _player->stop();
}

void DrumGuitarKey::jumpUpdateTime(int msec)
{
    int count = msec/_timeUnit;

    _startCount = count + 1;
    _curCount = 1;

    this->repaint();
}

void DrumGuitarKey::playEnd()
{
    _player->open(_media);
    _player->stop();
}


//drawDivision
//    int evWidth = rect.width()/_count;
//    for (int i=0; i<=_count; i++){

//        QLine line1(rect.x() + i*evWidth, rect.y() + rect.height()/2,
//                    rect.x() + i*evWidth, rect.y() + rect.height()/2 - SHOW_HEIGHT);
//        painter->drawLine(line1);
//        divs.append(QPoint(rect.x() + i*evWidth, rect.y() + rect.height()/2));
//        drawText(painter, QPoint(rect.x() + i*evWidth - 15, rect.y() + rect.height()/2 - SHOW_HEIGHT), QString::number(i + _startCount));
//        if (i == _count)
//            break;

//        int _height;
//        if ( ui->comboBox->currentText().compare("1/2") == 0
//             || ui->comboBox->currentText().compare("1/4") == 0
//             || ui->comboBox->currentText().compare("1/8") == 0
//             || ui->comboBox->currentText().compare("1/16") == 0){
//            QPoint _staP = QPoint(rect.x() + i*evWidth, rect.y() + rect.height()/2);
//            QPoint _endP = QPoint(rect.x() + (i+1)*evWidth, rect.y() + rect.height()/2);
//            QPoint _curP = equDivide(painter, _staP, _endP, QColor(255, 0, 0), SHOW_HEIGHT - 5);
//            _height = SHOW_HEIGHT - 5;
//            divs.append(_curP);

//            if ( ui->comboBox->currentText().compare("1/4") == 0
//                 || ui->comboBox->currentText().compare("1/8") == 0
//                 || ui->comboBox->currentText().compare("1/16") == 0){

//                QPoint _leftCurP = equDivide(painter, _staP, _curP, QColor(255, 0, 0), _height);
//                QPoint _rightCurP = equDivide(painter, _curP, _endP, QColor(255, 0, 0), _height);
//                _height = _height - 5;
//                divs.append(_leftCurP);
//                divs.append(_rightCurP);

//                if ( ui->comboBox->currentText().compare("1/8") == 0
//                     || ui->comboBox->currentText().compare("1/16") == 0){

//                    QPoint _leftLCurP = equDivide(painter, _staP, _leftCurP, QColor(255, 0, 0), _height);
//                    QPoint _leftRCurP = equDivide(painter, _leftCurP, _curP, QColor(255, 0, 0), _height);
//                    QPoint _rightLCurP = equDivide(painter, _curP, _rightCurP, QColor(255, 0, 0), _height);
//                    QPoint _rightRCurP = equDivide(painter, _rightCurP, _endP, QColor(255, 0, 0), _height);
//                    _height = _height - 5;
//                    divs.append(_leftLCurP);
//                    divs.append(_leftRCurP);
//                    divs.append(_rightLCurP);
//                    divs.append(_leftRCurP);

//                    if ( ui->comboBox->currentText().compare("1/16") == 0){

//                        divs.append(equDivide(painter, _staP, _leftLCurP, QColor(255, 0, 0), _height));
//                        divs.append(equDivide(painter, _leftLCurP, _leftCurP, QColor(255, 0, 0), _height));
//                        divs.append(equDivide(painter, _leftCurP, _leftRCurP, QColor(255, 0, 0), _height));
//                        divs.append(equDivide(painter, _leftRCurP, _curP, QColor(255, 0, 0), _height));

//                        divs.append(equDivide(painter, _curP, _rightLCurP, QColor(255, 0, 0), _height));
//                        divs.append(equDivide(painter, _rightLCurP, _rightCurP, QColor(255, 0, 0), _height));
//                        divs.append(equDivide(painter, _rightCurP, _rightRCurP, QColor(255, 0, 0), _height));
//                        divs.append(equDivide(painter, _rightRCurP, _endP, QColor(255, 0, 0), _height));
//                    }
//                }
//            }
//        }

//        else if ( ui->comboBox->currentText().compare("1/3") == 0
//                  || ui->comboBox->currentText().compare("1/6") == 0
//                  || ui->comboBox->currentText().compare("1/9") == 0
//                  || ui->comboBox->currentText().compare("1/12") == 0){

//            QPoint _staP = QPoint(rect.x() + i*evWidth, rect.y() + rect.height()/2);
//            QPoint _endP = QPoint(rect.x() + (i+1)*evWidth, rect.y() + rect.height()/2);
//            QList<QPoint> _curdivs = equDivide1(painter, _staP, _endP, QColor(255, 0, 0), SHOW_HEIGHT - 5);
//            _height = SHOW_HEIGHT - 5;
//            divs.append(_curdivs);

//            if (ui->comboBox->currentText().compare("1/9") == 0){

//                divs.append(equDivide1(painter, _staP, _curdivs.at(0), QColor(255, 0, 0), _height));
//                divs.append(equDivide1(painter, _curdivs.at(0), _curdivs.at(1), QColor(255, 0, 0), _height));
//                divs.append(equDivide1(painter, _curdivs.at(1), _endP, QColor(255, 0, 0), _height));
//            }

//            if ( ui->comboBox->currentText().compare("1/6") == 0
//                 || ui->comboBox->currentText().compare("1/12") == 0){

//               QPoint _leftP = equDivide(painter, _staP, _curdivs.at(0), QColor(255, 0, 0), _height);
//               QPoint _cenP =  equDivide(painter, _curdivs.at(0), _curdivs.at(1), QColor(255, 0, 0), _height);
//               QPoint _rightP =  equDivide(painter, _curdivs.at(1), _endP, QColor(255, 0, 0), _height);
//                _height = SHOW_HEIGHT - 5;
//                divs.append(_leftP);
//                divs.append(_cenP);
//                divs.append(_rightP);

//                if (ui->comboBox->currentText().compare("1/12") == 0){
//                    divs.append(equDivide(painter, _staP, _leftP, QColor(255, 0, 0), _height));
//                    divs.append(equDivide(painter, _leftP, _curdivs.at(0), QColor(255, 0, 0), _height));
//                    divs.append(equDivide(painter, _curdivs.at(0), _cenP, QColor(255, 0, 0), _height));
//                    divs.append(equDivide(painter, _cenP, _curdivs.at(1), QColor(255, 0, 0), _height));
//                    divs.append(equDivide(painter, _curdivs.at(1), _rightP, QColor(255, 0, 0), _height));
//                    divs.append(equDivide(painter, _rightP, _endP, QColor(255, 0, 0), _height));
//                }
//            }
//        }
//    }


//吉他
//ui->widget_rhythm->setHidden(true);
//int div = rect.height()/7;
//for (int i=5; i>=0; i--){
//    painter->drawLine(rect.x() + 60, rect.y() + div*(i+1), rect.x() + rect.width() - 60, rect.y() + div*(i+1));
//    YS.insert(5-i, rect.y() + div*(i+1));
//}

//int width = (rect.width()-120)/_count; //节宽度
//int num = getDivNum();
//int cell = width/num; //每个小单元格的宽度
//for (int index = 0; index<_count; index++){

//    painter->save();
//    painter->setPen(QPen(QBrush(QColor(255, 255, 0)), 2));
//    painter->drawLine(rect.x() + 60 + width*(index), rect.y() + div - 10,
//                      rect.x() + 60 + width*(index), rect.y() + rect.height() - div + 10);
//    XS.insert(index * getDivNum(), rect.x() + 60 + width*(index));
//    painter->restore();
//    drawText(painter, QPoint(rect.x() + 45 + width*(index), rect.y() + rect.height() - div + 25),
//             QString::number(index + _startCount));

//    int temp = num;
//    if (index+1 == _count)
//        temp = num + 1;
//    for (int i=1; i<temp; i++){
//        painter->drawLine(rect.x() + 60 + width*(index) + cell*i, rect.y() + div,
//                          rect.x() + 60 + width*(index) + cell*i, rect.y() + rect.height() - div);
//        XS.insert(index * getDivNum() + i, rect.x() + 60 + width*(index) + cell*i);

//        int textHeight;
//        if (i%2 == 0)
//            textHeight = rect.y() + rect.height() - div + 25 ;
//        else
//            textHeight = rect.y() + rect.height() - div;
//        drawText(painter, QPoint(rect.x() + 60 + width*(index) + cell*i - 15, textHeight),
//                 QString("%1/%2").arg(i).arg(getDivNum()));
//    }
//}
//}

//if (points.size() > 0){

//painter->save();
//painter->setPen(QPen(QBrush(QColor(0, 0, 0)), 2));
//painter->setBrush(QBrush(QColor(255, 0, 0)));
//for(int i=_startCount; i<_count+_startCount; i++){

//    foreach (PointM pm, points) {
//        if (pm.index == i){
//            if(_type == Type::dr){
//                qDebug() << " _curInd : " << (_curInd) % getDivNum()  << " cur beat : " << pm.curBeat;
//                qDebug() << " pm.index : " << pm.index << "  " << _startCount  <<  _curCount;
//                if(pm.curBeat == ((_curInd) % getDivNum()) && pm.index == _startCount + _curCount){
//                    foreach (int num, pm.nums) {
//                        painter->drawEllipse(drums[num]);
//                    }
//                }
//            } else if (_type == Type::gt){
//                foreach ( int num , pm.nums) {
//                    painter->drawRect(QRect(QPoint(XS.value((pm.index - _startCount) * getDivNum() + pm.curBeat),
//                                                   YS.value(num)),
//                                            QSize(XS.value(2) - XS.value(1), YS.value(1) - YS.value(2))));
//                }
//            } else if (_type == Type::kb){
//                int height = rect.height()/_count;
//                int reNum = getDivNum();
//                int _height;
//                int startP;
//                startP = rect.y() + rect.height() - (height*(pm.index - _startCount));
//                if (reNum == 16 || reNum == 8 || reNum == 4 || reNum == 2){
//                    _height = height/16;
//                } else if (reNum == 12 || reNum == 6 || reNum == 3){
//                    _height = height/12;
//                }
//                for (int i=0; i<pm.nums.size(); i++){
//                    painter->drawRect(QRect(QPoint(XS.value(pm.column - 1), startP - _height * pm.nums.at(i)),
//                                            QSize(XS.value(1) - XS.value(0), _height)));
//                }

//                ///
//                for (int i=0; i<pm.pnums.size(); i++){
////                            qDebug() << " column : " << pm.column << " size : " << pm.pnums.size();
//                    QPair<int, int> value  = pm.pnums.at(i);
//                    painter->drawRect(QRect(QPoint(XS.value(pm.column - 1), startP - _height * (value.second + value.first)),
//                                            QSize(XS.value(1) - XS.value(0), _height * (value.second + 1))));
//                }
//            }
//        }
//    }
//}

//painter->restore();
//}

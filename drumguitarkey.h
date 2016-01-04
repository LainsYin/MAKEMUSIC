#ifndef DRUMGUITARKEY_H
#define DRUMGUITARKEY_H

#include <QDialog>
#include <QMap>
#include <QPair>
#include <QMultiMap>
#include <QTimer>
#include <QWheelEvent>
#include <QJsonObject>
#include "mainwindow.h"
namespace Ui {
class DrumGuitarKey;
}

class PointM;
class QSettings;
class VlcInstance;
class VlcMedia;
class VlcMediaPlayer;

struct TimePoint{
    int     length; //持续时长
    int     time;
    QList<int> ids;    //拨动的哪些弦（多根弦）
};

class DrumGuitarKey : public QDialog
{
    Q_OBJECT

public:
    explicit DrumGuitarKey(QWidget *parent = 0);
    DrumGuitarKey(QWidget *parent, const QString &filePath);
    DrumGuitarKey(QWidget *parent,
                  const QString &name,
                  const QString &singer,
                  const QString &serial_id,
                  const QString &notes,
                  const QString &vPath,
                  const int &offset,
                  const int &bpm,
                  const int &type);
    void paintEvent(QPaintEvent *);
    void closeEvent(QCloseEvent *);
    void keyPressEvent(QKeyEvent *key);
    void mousePressEvent(QMouseEvent *mouse);
    void mouseReleaseEvent(QMouseEvent *mouse);
    void mouseMoveEvent(QMouseEvent *mouse);
    void wheelEvent(QWheelEvent *wheel);
    bool eventFilter(QObject *dialog, QEvent *event);
    ~DrumGuitarKey();

private slots:
    void on_pushButton_pause_clicked(bool checked);

    void on_comboBox_currentIndexChanged(int);

    void on_pushButton_plus_clicked();

    void on_pushButton_min_clicked();

    void on_pushButton_ret_clicked();

    void on_pushButton_save_clicked();

    void firstUpdateTime();

    void jumpUpdateTime(int msec);

    void playEnd();

private:
    void initWidget();
    void readAndSetStyleSheet();
    void initValue();

    void drawDivision(QPainter *painter);
    void drawInstrument(QPainter *painter);
    void drawText(QPainter *painter, const QPoint &sta, const QString &text);
    QPoint equDivide(QPainter *painter, const QPoint &staP, const QPoint &endP,
                     const QColor &color, const int  &height);

    QList<QPoint> equDivide1(QPainter *painter, const QPoint &staP, const QPoint &endP,
                             const QColor &color, const int  &height);
    int  getDivNum();

    bool isInDrum(const QPoint &point, const QPoint &circle,  const  int &radius);

    void changePlayerStatue();

    int isArea(const QPoint &point, const int &offset);
    bool drawSingle(QPainter *painter, const int &index,
                    const int &curbeat, const int &first, const int &second);

    bool orderPoints();
    QByteArray writeAppFile();
    void addPoints(TimePoint point, QList<TimePoint> &times);

    void selectPoint(QList<PointM> &points);

    bool delDouPoint(PointM &point);

private:
    Ui::DrumGuitarKey *ui;

    QJsonObject  _json;
    VlcInstance *_instance;
    VlcMedia *_media;
    VlcMediaPlayer *_player;
    QSettings *resUrl;
//    QList<QPoint> divs;
    int lines;
    int beatOfs;
    QStringList items;
    QString _name, _singer, _serial_id, _notes, _vPath, _resUrl;
    int _offset;
    int _bpm;
    int _type;

    int _count; //
    int _curCount;
    int _curInd;
    int _maxCount;
    int _startCount;
    int _index;
    bool isFirstOfset;

    int _timeUnit;
    QList<PointM> points;
    QMap<int, QRect> drums; ///
    QMap<int, int> XS;
    QMap<int, int> YS;

    QString typeStr;
    QTimer *timer;
    PointM multiP;
    bool startP;
    bool ctrl;
};

#endif // DRUMGUITARKEY_H

#ifndef YQCDELEGATE_H
#define YQCDELEGATE_H
#include <QApplication>
#include <QPainter>
#include <QTableView>
#include <QLineEdit>
#include <QStyleOption>
#include <QStyledItemDelegate>
#include <QItemDelegate>
#include <QDebug>
class QPen;
class QModelIndex;
class QPushButton;
class QStyleOptionViewItem;

class NoFocusDelegate : public QStyledItemDelegate
{
public:
    NoFocusDelegate();
    ~NoFocusDelegate();
    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const ;
};

class LibListDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit LibListDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event,
                     QAbstractItemModel *,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index);

    void setRGB(const int &_red, const int &_green, const int &_blue);
private:
    int red, green, blue;

};

class LibDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit LibDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event,
                     QAbstractItemModel *,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index);

    void setRGB(const int &_red, const int &_green, const int &_blue);
private:
    int red, green, blue;
};

class SetInfoDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    SetInfoDelegate(QObject *parent = 0);

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    bool editorEvent(QEvent *event, QAbstractItemModel *, const QStyleOptionViewItem &option, const QModelIndex &index);

//    QWidget *createEditor(QWidget *parent,
//                          const QStyleOptionViewItem &option,
//                          const QModelIndex &index) const;
//    void setEditorData(QWidget *editor, const QModelIndex &index) const;
//    void setModelData(QWidget *editor,
//                      QAbstractItemModel *model,
//                      const QModelIndex &index) const;
//    void updateEditorGeometry(QWidget *editor,
//                              const QStyleOptionViewItem &option,
//                              const QModelIndex &index) const;

    void setRowNum(const int &num);
signals:
    void left(const int &row);
    void right(const int &row);
    void currentRow(const int &row);
private:
    QColor _color;
    int rows;
};


class ButtonDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit ButtonDelegate(QObject *parent = 0);

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    bool editorEvent(QEvent *event,
                     QAbstractItemModel *,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setButtonText(const QString &text);
    void setTextColor(const int &r, const int &g, const int &b);
    void setLastNum(const int &num);
signals:
    void currentRow(const int &row);
public slots:
    void commitAndCloseEidt();

private:
//    QMap<QModelIndex, QStyleOptionButton*> m_btns; //
    QMap<QModelIndex, QStyleOptionViewItem> m_btns;
    QString _text;
    QColor _color;
};


class MoveDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit MoveDelegate(QObject *parent = 0);

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &) const;
    bool editorEvent(QEvent *event,
                     QAbstractItemModel *,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index);

    void setLeftText(const QString &str);
    void setRightText(const QString &str);
    void setLeftColor(int r, int g, int b);
    void setRightColor(int r, int g, int b);
signals:
    void left(const int &row);
    void right(const int &row);
public slots:

private:
    QString _text;

    QString leftText;
    QString rightText;
    QColor leftColor;
    QColor rightColor;
};


class ImageDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    ImageDelegate(QObject *parent = 0);

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex & index ) const;

    void setPixmap(const QString &str);
private:
    QPixmap _pixmap;
    QStringList list;
};


class IconDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    IconDelegate(QObject *parent = 0, bool isPlay = false);

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex & index ) const;
    bool editorEvent(QEvent *event,
                     QAbstractItemModel *,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index);

    void setPixmapNormal(const QString &str);
    void setPixmapPressed(const QString &str);
    void setXY(const int &_x, const int &_y);
//    void setPlay(const int &play);
signals:
    void currentRow(const int &row, int stateflag);
    bool currentRow(const QString &path);
private:
    QMap<QModelIndex, int> iconStatus;  
    QPixmap _pixmap_normal;
    QPixmap _pixmap_pressed;

    bool _Play;
    int x;
    int y;
};


class CheckBoxDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    CheckBoxDelegate(QObject *parent = 0);

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &  ) const;
    bool editorEvent(QEvent *event,
                     QAbstractItemModel *,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index);
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void setCheckBoxImage_normal(const QString &str);
    void setCheckBoximage_select(const QString &str);
    void setCheckBoxImage_disabled(const QString &str);

    void getIconStatus(QVector<int> &_rows);

    void setIconStatus(QVector<int> &_rows);

signals:
    void success();
public slots:
    void setAllSelect(bool checked = true);

private:
    QMap<QModelIndex, int> checkBoxStatus;
    QPixmap _pixmap_normal;
    QPixmap _pixmap_select;
    QPixmap _pixmap_disabled;
};


class HLineDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    HLineDelegate(QTableView* tableView){
        int gridHint = tableView->style()->styleHint(QStyle::SH_Table_GridLineColor, new QStyleOptionViewItemV4());
        QColor gridColor = static_cast<QRgb>(gridHint);
        pen = QPen(gridColor, 0, tableView->gridStyle());
        //    QColor color("red");
        //    color.setAlpha(0);
        //    pen.setColor(color);
        view = tableView;
    }
    ~HLineDelegate(){}
protected:
    void paint(QPainter* painter,const QStyleOptionViewItem& option,const QModelIndex& index) const
    {
        QStyleOptionViewItem itemOption(option);
        if(itemOption.state & QStyle::State_HasFocus)
            itemOption.state = itemOption.state ^ QStyle::State_HasFocus;
        QStyledItemDelegate::paint(painter,itemOption,index);
        QPen oldPen = painter->pen();
        painter->setPen(pen);
        //painter->drawLine(option.rect.topRight(),option.rect.bottomRight());
        painter->drawLine(itemOption.rect.bottomLeft(),itemOption.rect.bottomRight());
        painter->setPen(oldPen);
    }
private:
    QPen         pen;
    QTableView*  view;
};



class DelReconQueueDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    DelReconQueueDelegate(QObject *parent = 0): QItemDelegate(parent) { }
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
        const QModelIndex & index ) const
    {

        ////数据格式 currentValue, maxValue
       const QAbstractItemModel *itemModel = index.model();
       //获得索引对应Model中的数据
       QString value = itemModel->data(index, Qt::DisplayRole).toString();
       double finishedSubTaskNum, subTaskNum;
       if(value.isEmpty())
       {
            finishedSubTaskNum = 0;
            subTaskNum = 1;
       }
       else
       {
           QStringList values = value.split(",");
           finishedSubTaskNum = values.at(0).toDouble();
           subTaskNum =  values.at(1).toDouble();
           if(subTaskNum == 0)
               subTaskNum = 1;
       }

       //进度条的风格选项
       QStyleOptionProgressBarV2 *progressBarOption = new QStyleOptionProgressBarV2();
       QRect rect = QRect(option.rect.x(),
                          option.rect.y() + option.rect.height()/2-10,
                          option.rect.width(),
                          20);

       progressBarOption->rect = rect;
       progressBarOption->minimum = 0;
       progressBarOption->maximum = subTaskNum;
       progressBarOption->progress = finishedSubTaskNum;
       double t = (int)100*100*(finishedSubTaskNum/subTaskNum);
       int tt = t/100;
       if(finishedSubTaskNum >= subTaskNum)
           progressBarOption->text = QString("完成");
       else
           progressBarOption->text = QString::number(tt) + "%";
       progressBarOption->textVisible = true;
       progressBarOption->textAlignment = Qt::AlignCenter;
       //绘制进度条
       QApplication::style()->drawControl(QStyle::CE_ProgressBar, progressBarOption, painter);
    }
};



#endif // YQCDELEGATE_H

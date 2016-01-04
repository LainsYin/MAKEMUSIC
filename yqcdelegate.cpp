#include "yqcdelegate.h"
//#include "enuminfo.h"
#include <QColor>
#include <QMouseEvent>
#include <QMap>
#include <QVector>
#include <QEvent>
#include <QPushButton>
#include <QStyleOptionButton>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QFont>
#include <QLineEdit>
#include <QTextItem>
#include <QDebug>

enum checkBoxStatus{
    checkBox_normal=10,
    checkBox_select=20,
    checkBox_disabled=30,

    icon_normal=40,
    icon_pressed=50
};

NoFocusDelegate::NoFocusDelegate()
{

}

NoFocusDelegate::~NoFocusDelegate()
{

}

void NoFocusDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem itemOption(option);
     if (itemOption.state & QStyle::State_HasFocus)
     {
        itemOption.state = itemOption.state ^ QStyle::State_HasFocus;
     }
     QStyledItemDelegate::paint(painter, itemOption, index);
}


LibListDelegate::LibListDelegate(QObject *parent)
    :QItemDelegate(parent)
{
    red = 248;
    green = 248;
    blue = 240;
}

void LibListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QString info = index.data().toString();
    QStyleOptionViewItem  viewOption(option);
    if (viewOption.state & QStyle::State_HasFocus)
    {
        viewOption.state = viewOption.state ^ QStyle::State_HasFocus;
    }

    QRect rect = option.rect;
    painter->save();
    painter->setPen(QPen(QColor(red, green, blue, 100), 1, Qt::SolidLine, Qt::FlatCap));
    painter->setBrush(QBrush(QColor(red, green, blue), Qt::SolidPattern));
    painter->drawRect(rect);
    painter->restore();

//    painter->save();
//    painter->setPen(QPen(QColor(58, 58, 58)));
//    painter->drawText(rect, Qt::AlignLeft, info);
//    painter->restore();

}

bool LibListDelegate::editorEvent(QEvent *event, QAbstractItemModel *, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    return true;
}

void LibListDelegate::setRGB(const int &_red, const int &_green, const int &_blue)
{
    red = _red;
    green = _green;
    blue = _blue;
}



LibDelegate::LibDelegate(QObject *parent)
    :QItemDelegate(parent)
{
    red = 110;
    green = 161;
    blue = 240;
}

void LibDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStringList title_info = index.data().toString().split(",");
    QString title = title_info.value(1);
    QString info = title_info.value(2);

    if(title.isEmpty())
        return;

    QRect rect = option.rect;
    QRect rectTitle = QRect(rect.x(), rect.y(), 220, 36);
    QRect rectInfo = QRect(rect.x(), rect.y() + 36, 220, 54);
    painter->save();

    painter->setPen(QPen(QColor(red, green, blue), 1, Qt::SolidLine, Qt::FlatCap));
    painter->setBrush(QBrush(QColor(red, green, blue), Qt::SolidPattern));

    if(index.row() == 0 && index.column() == 0){
        QRect rectTemp = QRect(rect.x(), rect.y(), 220, 90);
        painter->drawRect(rectTemp);
    }else{
        painter->drawRect(rectTitle);
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(rectInfo);
    }

    painter->restore();

    painter->save();
    painter->setPen(QPen(QColor(255, 255, 255)));
    painter->drawText(rectTitle, Qt::AlignCenter, title);
    painter->restore();

    painter->save();
    if(index.row() == 0 && index.column() == 0)
        painter->setPen(QPen(QColor(255, 255, 255)));
    else
        painter->setPen(QPen(QColor(red, green, blue)));
    painter->drawText(rectInfo, Qt::AlignCenter, info);
    painter->restore();
}

bool LibDelegate::editorEvent(QEvent *event,
                              QAbstractItemModel *,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index)
{
    QRect rect = option.rect;
    QRect rectBu(rect.left() + rect.width()/2 - 30,
                 rect.top() + rect.height()/2 - 15,
                 60, 30);


    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* e =(QMouseEvent*)event;
        if (rectBu.contains(e->x(), e->y())) {
            int row = index.row();
            qDebug() << "row : " << row;
            return true;
        }
    }
    return false;
}

void LibDelegate::setRGB(const int &_red, const int &_green, const int &_blue)
{
    red = _red;
    green = _green;
    blue = _blue;
}



ImageDelegate::ImageDelegate(QObject *parent)
    :QItemDelegate(parent)
{

}

void ImageDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QPixmap pixmap(index.data().toString());
    QStyleOptionViewItem  viewOption(option);
    if (viewOption.state & QStyle::State_HasFocus)
    {
        viewOption.state = viewOption.state ^ QStyle::State_HasFocus;
    }

//    QItemDelegate::paint(painter, viewOption, index);

    QRect rect = option.rect;
    QRect pixmapRect(rect.left() + rect.width()/2 - 33,
                 rect.top() + rect.height()/2 - 33,
                 66, 66);
    painter->drawPixmap(pixmapRect, pixmap);
}

void ImageDelegate::setPixmap(const QString &str)
{
    QPixmap pixmap(str);
    _pixmap = pixmap;
}



ButtonDelegate::ButtonDelegate(QObject *parent)
    :QItemDelegate(parent)
{
    _color = QColor(108, 162, 234);
}

void ButtonDelegate::paint(QPainter *painter,
                           const QStyleOptionViewItem &option,
                           const QModelIndex &) const
{
    QStyleOptionViewItem button = option;//
    QRect rect = option.rect;
    QRect rectBu(rect.left() + rect.width()/2 - 25,
                 rect.top() + rect.height()/2 - 10,
                 50, 20);
    QPoint point;
    point.setX(rect.left()+ rect.width()/2 - 15);
    point.setY(rect.top() + rect.height()/2 + 5);
    painter->save();

    painter->setPen(_color);// (QColor(254, 107, 107)); //(254, 107, 107)
    button.displayAlignment = Qt::AlignCenter;
    painter->drawText(point, _text);
    this->drawFocus(painter, button, rectBu);

    painter->restore();
}

bool ButtonDelegate::editorEvent(QEvent *event, QAbstractItemModel *, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    QRect rect = option.rect;
    QRect rectBu(rect.left() + rect.width()/2 - 30,
                 rect.top() + rect.height()/2 - 20,
                 60, 40);

    if (event->type() == QEvent::MouseMove) {
        QMouseEvent* e =(QMouseEvent*)event;

        if (rectBu.contains(e->x(), e->y())) {

        }
    }

    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* e =(QMouseEvent*)event;
        if (rectBu.contains(e->x(), e->y())) {
            int row = index.row();
            emit currentRow(row);
            return true;
        }
    }
    return false;
}

QWidget *ButtonDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rect = option.rect;
    QRect rectBu(rect.left() + rect.width()/2,
                 rect.top() + rect.height()/2,
                 60, 40);

    if(index.column() == 7){
        QPushButton *button = new QPushButton(parent);
        button->setGeometry(rectBu);
        button->setObjectName("DeleteTextButton");
        button->setText("quxiao");
        connect(button, &QPushButton::clicked, this, &ButtonDelegate::commitAndCloseEidt);
        return button;
    }else{
        return QItemDelegate::createEditor(parent, option, index);
    }
}

void ButtonDelegate::setButtonText(const QString &text)
{
    _text = text;
}

void ButtonDelegate::setTextColor(const int &r, const int &g, const int &b)
{
    _color = QColor(r, g, b);
}

void ButtonDelegate::commitAndCloseEidt()
{
    QMessageBox::information(NULL, "title", "button");
}



MoveDelegate::MoveDelegate(QObject *parent)
    :QItemDelegate(parent)
{
    leftColor = QColor(108, 162, 234);
    rightColor = QColor(108, 162, 234);
    leftText = "上移";
    rightText = "下移";
}

void MoveDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &) const
{
    QStyleOptionViewItem button = option;
    QRect rect = option.rect;
    QRect rectLeft(rect.x() + rect.width()/4 - 8, rect.y() + rect.height()/2 -10, 40, 20);
    QRect rectRight(rect.x() + rect.width()*3/4 -8, rect.y() + rect.height()/2 -10 , 40, 20);

    QPoint pointUp, pointDown;
    pointUp.setX(rect.x() + rect.width()/4 - 5);
    pointUp.setY(rect.y() + rect.height()/2 + 5);
    pointDown.setX(rect.x() + rect.width()*3/4 -5);
    pointDown.setY(rect.y() + rect.height()/2 + 5);
    painter->save();

    painter->setPen(leftColor); //QColor(108, 162, 234) (254, 107, 107)
    painter->drawText(pointUp, leftText);
    painter->save();
    painter->setPen(rightColor);
    painter->drawText(pointDown, rightText);
    painter->restore();
//    this->drawDisplay(painter, button, rectBu, _text);
    this->drawFocus(painter, button, rectLeft);
    this->drawFocus(painter, button, rectRight);

    painter->restore();
}

bool MoveDelegate::editorEvent(QEvent *event, QAbstractItemModel *, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    QRect rect = option.rect;
    QRect rectLeft(rect.x() + rect.width()/4 - 8, rect.y() + rect.height()/2 - 5 , 30, 15);
    QRect rectRight(rect.x() + rect.width()*3/4 -8, rect.y() + rect.height()/2 - 5 , 30, 15);

    if (event->type() == QEvent::MouseMove) {
//        QMouseEvent* e =(QMouseEvent*)event;
    }

    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* e =(QMouseEvent*)event;
        int row = index.row();
        if (rectLeft.contains(e->x(), e->y())) {
            emit left(row);
            return true;
        }
        else if(rectRight.contains(e->x(), e->y())){
            emit right(row);
            return true;
        }
    }
    return false;
}

void MoveDelegate::setLeftText(const QString &str)
{
    leftText = str;
}

void MoveDelegate::setRightText(const QString &str)
{
    rightText = str;
}

void MoveDelegate::setLeftColor(int r, int g, int b)
{
    leftColor = QColor(r, g, b);
}

void MoveDelegate::setRightColor(int r, int g, int b)
{
    rightColor = QColor(r, g, b);
}





IconDelegate::IconDelegate(QObject *parent, bool isPlay)
    :QItemDelegate(parent)
{
    x = 5;
    y = 6;

    _Play = isPlay;
}

void IconDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
//    QString path = index.data().toString();
//    if(_Play)
//        (const_cast<IconDelegate *>(this))->paths.insert(index, path);
    int _switch = index.data().toInt();
    ///显示元单元格内容
//    QStyleOptionViewItem  viewOption(option);
//    if (viewOption.state & QStyle::State_HasFocus)
//    {
//        viewOption.state = viewOption.state ^ QStyle::State_HasFocus;
//    }

//    QItemDelegate::paint(painter, viewOption, index);

    int status = -1;
    QPixmap image;
    status = iconStatus.value(index);
    if(status != icon_normal && status != icon_pressed){
        (const_cast<IconDelegate *>(this))->iconStatus.insert(index, icon_normal);
        image = _pixmap_normal;
    }

    if(status == icon_normal)
        image = _pixmap_normal;
    else if(status == icon_pressed){
        image = _pixmap_pressed;
    }

    if(_switch != 0)
        image = _pixmap_pressed;

    QRect rect = option.rect;
    QRect pixmapRect(rect.left() + rect.width()/2 - x,
                 rect.top() + rect.height()/2 - y,
                 2*x, 2*y);
    painter->drawPixmap(pixmapRect, image);
}

bool IconDelegate::editorEvent(QEvent *event, QAbstractItemModel *, const QStyleOptionViewItem &option, const QModelIndex &index)
{

    QRect rect = option.rect;
    QRect pixmapRect(rect.left() + rect.width()/2 - x,
                 rect.top() + rect.height()/2 - y,
                 2*x, 2*y);

    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
    if (event->type() == QEvent::MouseButtonRelease && pixmapRect.contains(mouseEvent->pos()))
    {
        int row = index.row();
        int status = iconStatus.value(index);
        if (status == icon_normal){
            iconStatus.insert(index, icon_pressed);
            emit currentRow(row, true);
        }else if(status == icon_pressed){
            iconStatus.insert(index, icon_normal);
            emit currentRow(row, false);
        }

        if(_Play){
//            QString _path = paths.value(index);
//            qDebug() << "path : " << _path;
//            emit currentRow(_path);
        }

        return true;
    }
    return false;
}

void IconDelegate::setPixmapNormal(const QString &str)
{
    QPixmap pixmap(str);
    _pixmap_normal = pixmap;
}

void IconDelegate::setPixmapPressed(const QString &str)
{
    QPixmap pixmap(str);
    _pixmap_pressed = pixmap;
}

void IconDelegate::setXY(const int &_x, const int &_y)
{
    x = _x;
    y = _y;
}

//void IconDelegate::setPlay(const int &play)
//{

//}



CheckBoxDelegate::CheckBoxDelegate(QObject *parent)
    :QItemDelegate(parent)
{
    _pixmap_normal = QPixmap(":/other/images/checkbox_normal.png");
    _pixmap_select = QPixmap(":/other/images/checkbox_selected.png");
    _pixmap_disabled = QPixmap(":/other/images/checkbox_disable.png");
}

void CheckBoxDelegate::paint(QPainter *painter,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    QStyleOptionViewItem  viewOption(option);
    if (viewOption.state & QStyle::State_HasFocus)
    {
        viewOption.state = viewOption.state ^ QStyle::State_HasFocus;
    }

    QItemDelegate::paint(painter, viewOption, index);

    int status = -1;
    QPixmap image;
    status = checkBoxStatus.value(index);
    if(status != checkBox_normal
       && status != checkBox_select
       && status != checkBox_disabled){
        (const_cast<CheckBoxDelegate *>(this))->checkBoxStatus.insert(index, checkBox_normal);
        image = _pixmap_normal;
    }

    if(status == checkBox_normal)
        image = _pixmap_normal;
    else if(status == checkBox_select){
        image = _pixmap_select;
    }
    else if(status == checkBox_disabled)
        image = _pixmap_disabled;
//    else
//        return;   ///返回会造成 页面第一次视图不能加载代理图片，需要首次获取激活后才能正常使用

    painter->save();
    QRect rect = option.rect;
//    QRect pixmapRect(rect.left() + rect.width()*1/4 - 16,
//                 rect.top() + rect.height()/2 - 16,
//                 16, 16);

    QRect pixmapRect(QRect(rect.x()+8, rect.y() + rect.height()/2 - 8, 16, 16));

    painter->drawPixmap(pixmapRect, image);
    painter->restore();
}

bool CheckBoxDelegate::editorEvent(QEvent *event, QAbstractItemModel *, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    QRect rect = option.rect;
//    QRect pixmapRect(rect.left() + rect.width()*1/4 - 16,
//                 rect.top() + rect.height()/2 - 16,
//                 16, 16);
//    QRect pixmapRect(QRect(rect.x()+12, rect.y()+12, 16, 16));
    QRect pixmapRect(QRect(rect.x()+12, rect.y() + rect.height()/2 - 8, 16, 16));

    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
    if (event->type() == QEvent::MouseButtonRelease && pixmapRect.contains(mouseEvent->pos()))
    {
        int status = checkBoxStatus.value(index);
        if (status == checkBox_normal)
            checkBoxStatus.insert(index, checkBox_select);
        else if(status == checkBox_select)
            checkBoxStatus.insert(index, checkBox_normal);
    }

    return true;
}

void CheckBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QItemDelegate::setModelData(editor, model, index);
}

void CheckBoxDelegate::setCheckBoxImage_normal(const QString &str)
{
    QPixmap pixmap(str);
    _pixmap_normal = pixmap;
}

void CheckBoxDelegate::setCheckBoximage_select(const QString &str)
{
    QPixmap pixmap(str);
    _pixmap_select = pixmap;
}

void CheckBoxDelegate::setCheckBoxImage_disabled(const QString &str)
{
    QPixmap pixmap(str);
    _pixmap_disabled = pixmap;
}

void CheckBoxDelegate::setAllSelect(bool checked)
{
    QList<QModelIndex> index = checkBoxStatus.keys();
    for (int i=0; i<index.size(); i++){
        if(checked)
            checkBoxStatus.insert(index.at(i), checkBox_select);
        else
            checkBoxStatus.insert(index.at(i), checkBox_normal);
    }

    emit success();
}

void CheckBoxDelegate::getIconStatus(QVector<int> &_rows)
{
    QMap<QModelIndex, int>::iterator it = checkBoxStatus.begin();
    while(it != checkBoxStatus.end()){
        if (it.value() == checkBox_select)
            _rows.append(it.key().row());

        it++;
    }
}

void CheckBoxDelegate::setIconStatus(QVector<int> &_rows)
{
    QMap<QModelIndex, int>::iterator it = checkBoxStatus.begin();
    QVector<int>::iterator itRow = _rows.begin();
    for(int i=0; i<_rows.size(); i++)
    {

        while(it != checkBoxStatus.end())
        {
            if(it.key().row() == _rows.value(i))
            {
                checkBoxStatus.insert(it.key(), checkBox_normal);
            }

            it++;
        }
    }
}


SetInfoDelegate::SetInfoDelegate(QObject *parent): QItemDelegate(parent) {
    _color = QColor(108, 162, 234);
    rows = -1;
}

void SetInfoDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem button = option;
    QRect rect = option.rect;
    QRect rectAdd(rect.left() + rect.width()/2 - 15,
                 rect.top() + rect.height()/2 - 5,
                 30, 15);
    QRect rectLeft(rect.x() + rect.width()/4 - 8, rect.y() + rect.height()/2 - 5, 30, 15);
    QRect rectRight(rect.x() + rect.width()*3/4 -8, rect.y() + rect.height()/2 - 5 , 30, 15);

    if(index.row() != rows){

        QPoint pointUp, pointDown;
        pointUp.setX(rect.x() + rect.width()/4 - 8);
        pointUp.setY(rect.y() + rect.height()/2 + 5);
        pointDown.setX(rect.x() + rect.width()*3/4 -8);
        pointDown.setY(rect.y() + rect.height()/2 + 5);
        painter->save();

        painter->setPen(QColor(108, 162, 234)); //QColor(108, 162, 234) (254, 107, 107)
        painter->drawText(pointUp, "修改");
        painter->save();
        painter->setPen(QColor(254, 107, 107));
        painter->drawText(pointDown, "删除");
        painter->restore();
        //    this->drawDisplay(painter, button, rectBu, _text);
        this->drawFocus(painter, button, rectLeft);
        this->drawFocus(painter, button, rectRight);

        painter->restore();
    }

    if(index.row() == rows){
        QPoint point;
        point.setX(rect.left()+ rect.width()/2 - 12);
        point.setY(rect.top() + rect.height()/2 + 5);
        painter->save();

        painter->setPen(QColor(108, 162, 234));// (QColor(254, 107, 107)); //(254, 107, 107)
        button.displayAlignment = Qt::AlignCenter;
        painter->drawText(point, "添加");
        this->drawFocus(painter, button, rectAdd);

        painter->restore();
    }
}

bool SetInfoDelegate::editorEvent(QEvent *event, QAbstractItemModel *, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    QRect rect = option.rect;
    QRect rectLeft(rect.x() + rect.width()/4 - 8, rect.y() + rect.height()/2 - 5 , 30, 15);
    QRect rectRight(rect.x() + rect.width()*3/4 -8, rect.y() + rect.height()/2 - 5 , 30, 15);
    QRect rectAdd(rect.left() + rect.width()/2 - 15,
                 rect.top() + rect.height()/2 - 5,
                 30, 15);

    if (event->type() == QEvent::MouseMove) {
//        QMouseEvent* e =(QMouseEvent*)event;
    }

    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* e =(QMouseEvent*)event;
        int row = index.row();

        if(row != rows){

            if (rectLeft.contains(e->x(), e->y())) {
                emit left(row);
                return true;
            }else if(rectRight.contains(e->x(), e->y())){
                emit right(row);
                return true;
            }
        }

        if(row == rows){
            if(rectAdd.contains(e->x(), e->y())){
                emit currentRow(row);
                return true;
            }
        }
    }
    return false;
}

void SetInfoDelegate::setRowNum(const int &num)
{
    rows = num - 1;
}

//QWidget *SetInfoDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
//{
//    if(index.column() == 1){
//        QLineEdit *editor = new QLineEdit(parent);
//        return editor;
//    }else
//        QItemDelegate::createEditor(parent, option, index);
//}

//void SetInfoDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
//{
//    if(index.column() == 1){
//    QString text = index.model()->data(index, Qt::EditRole).toString();
//    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
//    lineEdit->setText(text);
//    }
//}

//void SetInfoDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
//{
//    if(index.column() == 1){
//    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
//    QString text = lineEdit->text();
//    model->setData(index, text, Qt::EditRole);
//    }
//}

//void SetInfoDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
//{
//    if(index.column() == 1)
//    editor->setGeometry(option.rect.x() -10,
//                        option.rect.y() -10,
//                        30, 20);

//}

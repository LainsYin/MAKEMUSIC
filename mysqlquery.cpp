#include "mysqlquery.h"
#include "mainwindow.h"
//#include "kmatch.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSettings>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlField>
#include <QDateTime>

#include <QDir>
#include <QFile>
#include <QCoreApplication>

#include <QDebug>
#include <QTime>
#include <QMessageBox>
#define BLACK_YES 1
#define BLACK_NO  0

MysqlQuery::MysqlQuery()
{
    initConfig = new QSettings("config.ini", QSettings::IniFormat);
    initConfig->setIniCodec("UTF-8");

    _query = NULL;
    readConfigFile();   
}

MysqlQuery::~MysqlQuery()
{

}

void MysqlQuery::readConfigFile()
{
    hostName = initConfig->value("SQL/hostname").toString();
    port = initConfig->value("SQL/port").toString();
    userName = initConfig->value("SQL/username").toString();
    password = initConfig->value("SQL/password").toString();
    dataBase = initConfig->value("SQL/database").toString();
    infoBase = initConfig->value("SQL/info").toString();
}

bool MysqlQuery::openMysql(const QString databaseName)
{    
    if (QSqlDatabase::contains(dataBase)) //"yqcdb"
        db = QSqlDatabase::database(dataBase);
    else
        db = QSqlDatabase::addDatabase("QMYSQL");

    db.setHostName(hostName);
    db.setPort(port.toInt());
    db.setDatabaseName(dataBase);
    db.setUserName(userName);
    db.setPassword(password);

    if (!db.open()){        
        QString error =  QString("数据库登录失败！\n%1").arg(db.lastError().text());
        QMessageBox::information(NULL, "错误提示", error, QMessageBox::Ok);
//        KMatch::writeLog(QString("%1 connection failure.").arg(databaseName));
//        exit(0);
        return false;
    }else{
//        KMatch::writeLog(QString("%1 connection successful.").arg(databaseName));
        qDebug() << "connection successful." << "  databaseName : " << databaseName;
    }

    _query = new QSqlQuery(db);
    return true;
}

void MysqlQuery::closeMysql(const QString databaseName)
{
    qDebug() << "databaseName : " << databaseName;
    if (db.isOpen()){

        QString connect = QSqlDatabase::database().connectionName();
        db.close();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(connect);
    }
}

bool MysqlQuery::openInfosql(const QString databaseName)
{
    if (QSqlDatabase::contains(infoBase)) //"yqcdb"
        dbInfo = QSqlDatabase::database(infoBase);
    else
        dbInfo = QSqlDatabase::addDatabase("QMYSQL");

    dbInfo.setHostName(hostName);
    dbInfo.setPort(port.toInt());
    dbInfo.setDatabaseName(infoBase);
    dbInfo.setUserName(userName);
    dbInfo.setPassword(password);

    if (!dbInfo.open()){
        QString error =  QString("数据库登录失败！\n%1").arg(db.lastError().text());
        QMessageBox::information(NULL, "错误提示", error, QMessageBox::Ok);
//        exit(0);
        return false;
    }else{
        qDebug() << "connection successful." << "  databaseName : " << databaseName;
    }

    _queryInfo = new QSqlQuery(dbInfo);
    return true;
}

void MysqlQuery::closeInfosql(const QString databaseName)
{
    qDebug() << "databaseName : " << databaseName;
    if (dbInfo.isOpen()){

        QString connect = QSqlDatabase::database().connectionName();
        dbInfo.close();
        dbInfo = QSqlDatabase();
        QSqlDatabase::removeDatabase(connect);
    }
}

bool MysqlQuery::isMatch(const QString &name, const QString &serial_id)
{
    QString serial = "";
    if(!serial_id.isEmpty())
        serial = QString(" AND `serial_id` = %1").arg(serial_id);

    QString sqlstr = QString("SELECT count(*) as _count FROM `media` WHERE `name` = '%1' %2 AND `match` = 1;")
                                    .arg(name)
                                    .arg(serial);
    qDebug() << " match " << sqlstr;

   if(_query->exec(sqlstr)){
        if(_query->next()){
            if(_query->value("_count").toInt() > 1)
                return true;
        }
    }
    else
        return false;
}

bool MysqlQuery::queryMedia(const QString &search, QSqlQuery &query)
{
    QString queryStr = QString(
                        " select mm.serial_id as _serial_id, mm.name as _name, mm.singer as _singer, "
                        " mmll.detail as _language, tt.detail as _type, mm.path as _path, mm.enabled as _enabled, "
                        " mm.path as _path, mm.mid as _mid, mm.match as _match, mm.count as _count "
                        " from media as mm "
                        " left join media_language as mmll on mm.language = mmll.id "
                        " left join media_type as tt on mm.type = tt.id "
                        " where  mm.name like '%%1%' or mm.pinyin like '%%1%' "
                        "        or mm.singer like '%%1%' and mm.enabled = 1 and black = 0 "
                        " order by mm.count desc, mm.name desc ;"
                        )
                        .arg(search);

    QTime time;
    time.start();
//    qDebug() << "queryStr : " << queryStr;
    bool ok = querySql(queryStr, query);
    qDebug()<< time.elapsed()/1000.0<<"s";
    return ok;
}


bool MysqlQuery::querySql(const QString &queryStr, QSqlQuery &query)
{
    QSqlQuery _query; //(db);
   if(_query.exec(queryStr)){
        query = _query;
        return true;
    }
    else
       return false;
}

bool MysqlQuery::queryReward(QSqlQuery &_query)
{
    QSqlQuery query;
    if(query.exec("SELECT * FROM `ktv_game_reward` ORDER BY `starttime` DESC;")){
        _query = query;
        return true;
    }
    else
        return false;
}

qint64 MysqlQuery::getMaxIds(const qint64 &datetime)
{
    qint64 ids = datetime*100;
    QString sqlStr = QString("SELECT * FROM `ktv_game_reward` "
                             "WHERE `ids` >= %1 "
                             "ORDER BY `starttime` DESC;").arg(ids);
    if(_queryInfo->exec(sqlStr)){
        if(_queryInfo->next())
            return _queryInfo->value("ids").toLongLong();
        else
            return -1;
    }
    else
        return -1;
}


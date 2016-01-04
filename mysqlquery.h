#ifndef MYSQLQUERY_H
#define MYSQLQUERY_H

#include <QSqlDatabase>
#include <QString>
class MediaList;
class QSettings;
class RewardInfo;
class MysqlQuery
{
public:
    MysqlQuery();
    ~MysqlQuery();

public:
    void readConfigFile();
    bool openMysql(const QString databaseName="yiqiding_ktv");
    void closeMysql(const QString databaseName="yiqiding_ktv");
    bool openInfosql(const QString databaseName="yiqiding_info");
    void closeInfosql(const QString databaseName="yiqiding_info");

    bool isMatch(const QString &name, const QString &serial_id = NULL);

    bool queryMedia(const QString &search, QSqlQuery &query);
    bool querySql(const QString &queryStr, QSqlQuery &query);
    bool queryReward(QSqlQuery &_query);

    qint64 getMaxIds(const qint64 &datetime);


public:
    QString hostName;
    QString port;
    QString databaseName;
    QString userName;
    QString password;
    QString dataBase;
    QString infoBase;

private:

public:
    QSqlDatabase db;
    QSettings *initConfig;
    QSqlQuery *_query;

    QSqlDatabase dbInfo;
    QSqlQuery *_queryInfo;
};
#endif // MYSQLQUERY_H

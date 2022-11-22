#include "BugLog.h"

BugLog::BugLog()
{
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE");//sqlLite
    db.setDatabaseName("test.db");
    QString s = db.connectionName();
    db.open();//打开数据库，没有则创建

    QSqlQuery *psql;
    psql = new QSqlQuery();
    //psql->exec();
    //执行sql语句
    //test
    psql->exec("create table user(name,age)");
    psql->exec("insert into user(name,age) values('zhangsan',23)");
}

BugLog::~BugLog()
{

}

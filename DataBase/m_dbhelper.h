#ifndef M_DBHELPER_H
#define M_DBHELPER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QFile>

class M_DbHelper : public QObject
{
    Q_OBJECT
public:
    explicit M_DbHelper(QObject *parent = nullptr);

    //连接到数据库
    static bool ConnectDataBase(QString Type, QString DbName);
    static bool ConnectDataBase(QString Type, QString DbName, QString HostName,QString user, QString Password, int Port = int(), QString Options = QString());
    //获取本地数据库驱动
    static QStringList GetDriver(void);
    //判断当前数据库是否已经连接
    static bool IsConnect(QString Dbname);
    //新增表单到数据库
    static bool CreateNewTable(QString tableName,QStringList Data);
    static bool CreateNewTable(QString tableName,QStringList DataName,QStringList DataType);
    //添加内容到指定表单
    static bool AddDataToTable(QString tableName,QStringList Values);
    static bool AddDataToTable(QString tableName,QStringList DataName,QStringList Values);
    //新增数据库文件
    static bool CreateDataBaseFile(QString filename,QString filePath);
    //查询是否存在指定表
    static bool IsEmpty_Table(QString tableName);
    //查询指定表内数据

private:

    static QString dbname;
//    bool IsConnect;
signals:

};

#endif // M_DBHELPER_H

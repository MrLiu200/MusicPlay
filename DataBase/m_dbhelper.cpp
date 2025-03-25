#include "m_dbhelper.h"

M_DbHelper::M_DbHelper(QObject *parent) : QObject(parent)
{

}

bool M_DbHelper::ConnectDataBase(QString Type, QString DbName)
{
    QSqlDatabase dbConn;
    dbConn = QSqlDatabase::addDatabase(Type);
    dbConn.setDatabaseName(DbName);
    return dbConn.open();
}

bool M_DbHelper::ConnectDataBase(QString Type, QString DbName, QString HostName, QString user, QString Password, int Port, QString Options)
{
    bool ret = false;
    if(IsConnect(DbName)) return ret;

    QSqlDatabase dbConn;
    dbConn = QSqlDatabase::addDatabase(Type);
    dbConn.setHostName(HostName);
    dbConn.setDatabaseName(DbName);
    dbConn.setUserName(user);
    dbConn.setPassword(Password);
    dbConn.setPort(Port);
    dbConn.setConnectOptions(Options);


    return dbConn.open();
}

QStringList M_DbHelper::GetDriver()
{
    QStringList ret = QSqlDatabase::drivers();
    foreach(QString driver, ret){
        qDebug()<<driver;
    }
    return ret;
}

bool M_DbHelper::IsConnect(QString Dbname)
{
    return QSqlDatabase::contains(Dbname);
}

bool M_DbHelper::CreateNewTable(QString tableName, QStringList Data)
{
    bool ret = false;
    QString str,temp;
    QSqlQuery query;
    if((tableName.isEmpty()) || (Data.isEmpty())){
        goto p1;
    }
    str = "create table if not exists " +tableName + "(";
    for(int i=0;i<Data.size()-1;i++){
        temp = Data.at(i) + ",";
        str += temp;
    }
    temp = Data.at(Data.size()-1);
    str += temp + ")";
    qDebug()<<str;
    ret = query.exec(str);
p1:
    return ret;
}

bool M_DbHelper::CreateNewTable(QString tableName, QStringList DataName, QStringList DataType)
{
    bool ret = false;
    QString str,temp;
    QSqlQuery query;
    if((tableName.isEmpty()) || (DataName.isEmpty()) || (DataType.isEmpty()) ||(DataName.size() != DataType.size())){
        goto p1;
    }

    str = "create table if not exists " +tableName + "(";
    for(int i=0;i<DataName.size()-1;i++){
        temp = DataName.at(i) + " " + DataType.at(i) + ",";
        str += temp;
    }
    temp = DataName.at(DataName.size()-1) + " " + DataType.at(DataName.size()-1);
    str += temp + ")";

    ret = query.exec(str);
p1:
    return ret;
}

bool M_DbHelper::AddDataToTable(QString tableName, QStringList Values)
{
    bool ret = false;
    QString sql;
    QSqlQuery query;
    //判断values是否为空
    if(Values.isEmpty()){
        goto p1;
    }

    //判断table 是否存在
    if(!IsEmpty_Table(tableName)){
        goto p1;
    }

    sql.clear();
    sql = "insert into " + tableName + " values(";
    for(int i=0;i<Values.size()-1;i++){
        sql += "'" + Values.at(i) + "',";
    }
    sql += "'" + Values.at(Values.size()-1) + "');";
    ret = query.exec(sql);
p1:
    return ret;
}

bool M_DbHelper::AddDataToTable(QString tableName, QStringList DataName, QStringList Values)
{
    bool ret = false;
    QString sql;
    QSqlQuery query;
    //判断数据是否为空
    if(Values.isEmpty() || DataName.isEmpty()){
        goto p1;
    }
    //判断table 是否存在
    if(!IsEmpty_Table(tableName)){
        goto p1;
    }

    sql.clear();
    sql = "insert into " + tableName +"(";
    for(int i=0;i<DataName.size()-1;i++){
        sql += DataName.at(i) + ")";
    }
    sql += DataName.at(DataName.size()-1) + " values(";
    for(int i=0;i<Values.size()-1;i++){
        sql += "'" + Values.at(i) + "',";
    }
    sql += "'" + Values.at(Values.size()-1) + "');";
    ret = query.exec(sql);
p1:
    return ret;
}

bool M_DbHelper::CreateDataBaseFile(QString filename, QString filePath)
{
    QString name = filePath + "/" + filename;
    QFile file(name);
    bool create = true,ret = false;
    if(file.exists()){
        if(QMessageBox::question(nullptr,"询问","是否删除原文件") == QMessageBox::Yes){
            create = true;
        }else{
            create = false;
        }
    }
    qDebug()<<name;
    if(create){
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            // 如果无法打开文件，输出错误信息
            QMessageBox::warning(nullptr,"error","创建失败: " + file.errorString());
            goto p1;
        }

        // 写入一个空字符串到文件，这将清空文件内容
        QTextStream stream(&file);
        stream << "";

        // 关闭文件
        file.close();
        ret = true;
    }
p1:
    return ret;
}

bool M_DbHelper::IsEmpty_Table(QString tableName)
{
    QString sql;
    QSqlQuery query;
    sql = QString("select * from sqlite_master where name='%1'").arg(tableName);
    query.exec(sql);
    return query.next();
}

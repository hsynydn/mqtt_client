#include "dbManagement.h"

DBManager::DBManager(){

    databaseName = "mydb.db";

    DATABASE_CONNECTION_STATUS = false;

    // Create an SQLite database
    database = QSqlDatabase::addDatabase("QSQLITE");

    QString path = QDir::currentPath() + "/" + databaseName;

    qDebug() << "Database Path :: " << path;

    database.setDatabaseName(path);

    query = QSqlQuery(database);
}

int DBManager::connect(){

    int retVal = RET_OK;

    if(DATABASE_CONNECTION_STATUS){

        qDebug() << "Database Connection Request not completed. Already Connected";
        return RET_OK;
    }

    if (database.open()) {

        qDebug() << "Database Opened";

        DATABASE_CONNECTION_STATUS = true;

    }else{

        qDebug() << "Database Not Opened";
        retVal = RET_FAIL;

    }

    return retVal;
}

void DBManager::close(){
    database.close();
}

bool DBManager::run_query(std::string query_text){

    bool retVal;
    retVal = query.exec(QString::fromUtf8(query_text.c_str()));
    return retVal;
}

bool DBManager::hasNext(){
    return query.next();
}

Setting DBManager::getSetting(){

    Setting s;

    s.serial = query.value(0).toString();
    s.version = query.value(1).toString();
    s.product = query.value(2).toString();

    if (query.value(3) == 1) {
        s.demo = true;
    }else{
        s.demo = false;
    }

    s.brokerName = query.value(4).toString();
    s.brokerPort = query.value(5).toInt();

    return s;
}

Operator DBManager::getOperator(){

    Operator o;

    o.id = query.value(0).toInt();
    o.fullname = query.value(1).toString();
    o.password = query.value(2).toString();
    o.phone = query.value(3).toString();
    o.createdDate = query.value(4).toString();
    o.emailId = query.value(5).toInt();

    return o;
}

Sensor DBManager::getSensor(){

    Sensor s;

    s.id = query.value(0).toInt();
    s.maxVal = query.value(1).toInt();
    s.minVal = query.value(2).toInt();
    s.name = query.value(3).toString();

    return s;
}

Topic DBManager::getTopic(){

    Topic t;

    t.id = query.value(0).toInt();
    t.name = query.value(1).toString();

    return t;
}

Email DBManager::getEmail(){

    Email e;

    e.id = query.value(0).toInt();
    e.email = query.value(1).toString();

    return e;
}

std::string DBManager::FX_QUERY_OPERATOR_BY_NAME(QString name){

    QString result = QUERY_OPERATOR_BY_NAME;
    result += QString("\"");
    result += QString(name);
    result += QString("\";");

    return result.toStdString().c_str();
}

std::string DBManager::FX_QUERY_EMAIL_BY_ID(int id){

    QString result = QUERY_EMAILS_BY_ID;
    //result += QString("\"");
    result += QString::number(id);
    //result += QString("\";");
    result += QString(";");

    return result.toStdString().c_str();
}

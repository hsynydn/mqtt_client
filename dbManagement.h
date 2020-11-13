#ifndef DBMANAGEMENT_H
#define DBMANAGEMENT_H

#include <string>
#include <QString>
#include <QDir>
#include <QDebug>
#include <QVariant>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include "customTypes.h"

#define QUERY_OPERATORS "SELECT * FROM Operators;"
#define QUERY_TOPICS "SELECT * FROM Topics;"
#define QUERY_SENSORS "SELECT * FROM Sensors;"
#define QUERY_EMAILS_BY_ID "SELECT * FROM Emails WHERE email_id = "
#define QUERY_SETTINGS "SELECT * FROM Settings;"
#define QUERY_OPERATOR_BY_NAME "SELECT * FROM Operators WHERE fullname = "

struct _setting{
    QString product;
    QString serial;
    QString version;
    bool demo;
    QString brokerName;
    int brokerPort;
}typedef Setting;

struct _operator{
    int id;
    QString fullname;
    QString password;
    QString phone;
    QString createdDate;
    int emailId;
}typedef Operator;

struct _sensor{
    int id;
    int maxVal;
    int minVal;
    QString name;
}typedef Sensor;

struct _email{
    int id;
    QString email;
}typedef Email;

struct _topic{
    int id;
    QString name;
}typedef Topic;

class DBManager {

    public:

        DBManager();

        int     connect();
        void    close();

        bool    run_query(const std::string query);
        bool    hasNext();

        Operator    getOperator();
        Email       getEmail();
        Topic       getTopic();
        Sensor      getSensor();
        Setting     getSetting();

        static std::string FX_QUERY_OPERATOR_BY_NAME(QString name);
        static std::string FX_QUERY_EMAIL_BY_ID(int id);

    private:

        QString databaseName;
        QSqlDatabase database;
        QSqlQuery query;

        bool DATABASE_CONNECTION_STATUS;
};

#endif // DBMANAGEMENT_H

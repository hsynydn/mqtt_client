#include "mainwindow.h"

#include <string>
#include <QtCore>

#include <ctime>
#include <ctype.h>
#include <stdio.h>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#define QOS         1
#define TIMEOUT     10000L

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");

    QString path = QDir::currentPath() + "/mydb.db";

    qDebug() << path;

    database.setDatabaseName(path);

    bool retVal = database.open();

    if(retVal){

        qDebug() << "Database Opened";

        QSqlQuery query;

        query.exec("SELECT * FROM Settings");

        while (query.next()) {
            QString broker_name = query.value(4).toString();
            QString broker_port = query.value(5).toString();

            ui->lineEdit_host_name->setText(broker_name);
            ui->lineEdit_broker_port->setText(broker_port);
        }

        query.exec("SELECT * FROM Topics");

        while (query.next()) {
            QString topic_name = query.value(1).toString();

            ui->listWidget_topic_list->addItem(topic_name);
        }

        ui->label_none->setText(ui->listWidget_topic_list->item(0)->text());

        query.exec("SELECT * FROM Operators");

        while (query.next()) {
            QString operator_name = query.value(1).toString();

            ui->comboBox_user->addItem(operator_name);
        }

        query.exec("SELECT * FROM Sensors");

        QDoubleSpinBox* sensor_list[3];
        sensor_list[0] = ui->doubleSpinBox_1;
        sensor_list[1] = ui->doubleSpinBox_2;
        sensor_list[2] = ui->doubleSpinBox_3;

        QLabel* sensor_name_list[3];
        sensor_name_list[0] = ui->label_sensor_1;
        sensor_name_list[1] = ui->label_sensor_2;
        sensor_name_list[2] = ui->label_sensor_3;


        int counter = 0;

        while(query.next()){

            int max_val = query.value(1).toInt();
            int min_val = query.value(2).toInt();
            QString sensor_name = query.value(3).toString();

            sensor_list[counter]->setRange(min_val, max_val);
            sensor_name_list[counter]->setText(sensor_name);

            counter++;
        }


    }else{

        qDebug() << "Database Connection Failed\n";

        ui->lineEdit_host_name->setText("broker.hivemq.com");
        ui->lineEdit_broker_port->setText("1883");

        ui->listWidget_topic_list->addItem("metosoft/topic_2");
        ui->listWidget_topic_list->addItem("metosoft/topic_3");


        ui->comboBox_user->addItem("Hüseyin Aydın");
        ui->comboBox_user->addItem("Ayhan Demirhan");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_connect_clicked()
{
    broker_address.clear();
    broker_address.insert(broker_address.length(), "tcp://");
    broker_address.insert(broker_address.length(), ui->lineEdit_host_name->text());
    broker_address.insert(broker_address.length(), ":");
    broker_address.insert(broker_address.length(), ui->lineEdit_broker_port->text());

    qDebug() << "Broker Address ::: " << broker_address << "\n";

    conn_opts = MQTTClient_connectOptions_initializer;

    int returnCode;

    MQTTClient_create(&mqttClient,
                      broker_address.toStdString().c_str(),
                      client_id.toStdString().c_str(),
                      MQTTCLIENT_PERSISTENCE_DEFAULT, NULL);

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    if ((returnCode = MQTTClient_connect(mqttClient, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", returnCode);
        exit(-1);
    }else{

        ui->pushButton_connect->setAutoFillBackground(true);
        ui->pushButton_connect->setText("Connected");
        ui->pushButton_connect->setStyleSheet("color: rgb(0, 255, 0)");
        ui->pushButton_connect->update();
    }
}

void MainWindow::on_pushButton_send_clicked()
{

    qDebug() << "Check Point 1\n";

    int returnCode;

    topic = ui->label_none->text();

    QString payload = getPayloadAsJson();
    //QString payload = "PAYLOAD";

    qDebug().noquote() << "THIS IS PAYLOAD --------- \n" << payload;

    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;

    pubmsg.payload = (void*)(payload.toStdString().c_str());
    pubmsg.payloadlen = strlen(payload.toStdString().c_str());

    qDebug() << "Length of Payload ::: " << pubmsg.payloadlen;

    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    MQTTClient_publishMessage(mqttClient,
                              topic.toStdString().c_str(),
                              &pubmsg,
                              &token);

    /*
    printf("Waiting for up to %d seconds for publication of %s\n on topic %s for client with ClientID: %s\n",
           (int)(TIMEOUT/1000),
           payload.toStdString().c_str(),
           topic.toStdString().c_str(),
           client_id.toStdString().c_str());
    */

    returnCode = MQTTClient_waitForCompletion(mqttClient, token, TIMEOUT);

    printf("Message with delivery token %d delivered\n", token);
}

void MainWindow::on_listWidget_topic_list_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    ui->label_none->setText(current->text());
}

void MainWindow::on_MainWindow_destroyed()
{
    MQTTClient_disconnect(mqttClient, 10000);
    MQTTClient_destroy(&mqttClient);
}

QString MainWindow::getPayloadAsJson()
{

   time_t now = time(0);
   char* dt = ctime(&now);
   *(dt+24) = '\0';

   QSqlQuery query;
   query.exec("SELECT * FROM Settings");

   QString serial, version, product;
   int demo;

   while (query.next()) {
       serial = query.value(0).toString();
       version = query.value(1).toString();
       product = query.value(2).toString();
       demo = query.value(3).toInt();
   }

   QString message;


   message += "{\n";

   message += "\"product\": \"";
   message += product;
   message += " \",\n";

   message += "\"version\": \"";
   message += version;
   message += " \",\n";

   message += "\"date\": \"";
   message += dt;
   message += "\",\n";

   message += "\"serial\": \"";
   message += serial;
   message += "\",\n";

   if(demo == 1){
       message += "\"demo\": \"";
       message += "true";
       message += " \",\n";
   }else{
       message += "\"demo\": \"";
       message += "false";
       message += " \",\n";
   }

   QString selected_operator_name = ui->comboBox_user->currentText();
   query.exec("SELECT * FROM Operators WHERE fullname = \"" + selected_operator_name + "\";");

   int operator_id;
   QString fullname;
   QString password;
   QString phone;
   int email_id;

   while (query.next()) {

       operator_id = query.value(0).toInt();
       qDebug() << operator_id;

       fullname = query.value(1).toString();
       qDebug() << fullname;

       password = query.value(2).toString();
       qDebug() << password;

       phone = query.value(3).toString();
       qDebug() << phone;

       email_id = query.value(5).toInt();
       qDebug() << email_id;
   }



   query.exec("SELECT * FROM Emails WHERE email_id = " + QString::number(email_id) + ";");
   query.next();

   QString email = query.value(1).toString();

   message += "\"operator\": { ";

   message += "\"id\": \"";
   message += QString::number(operator_id);
   message += "\", ";

   message += "\"name\": \"";
   message += fullname;
   message += "\", ";

   message += "\"phone\": \"";
   message += phone;
   message += "\", ";

   message += "\"email\": [\"";
   message += email;
   message += "\"] }, \n";

   message += "\"sensor\": [";

   QString lab_sn1 = ui->label_sensor_1->text();
   QString lab_sn2 = ui->label_sensor_2->text();
   QString lab_sn3 = ui->label_sensor_3->text();

   double lab_sn1out = ui->doubleSpinBox_1->value();
   double lab_sn2out = ui->doubleSpinBox_2->value();
   double lab_sn3out = ui->doubleSpinBox_3->value();

   message += "{ \"";
   message += lab_sn1;
   message += "\" : \"";
   message += QString::number(lab_sn1out);
   message += "\"},";

   message += "{ \"";
   message += lab_sn2;
   message += "\" : \"";
   message += QString::number(lab_sn2out);
   message += "\"},";

   message += "{ \"";
   message += lab_sn3;
   message += "\" : \"";
   message += QString::number(lab_sn3out);
   message += "\"} ]\n";

   message += "}";

   return message;
}

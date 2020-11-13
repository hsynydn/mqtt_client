#include "mainwindow.h"
#include "msoft/json.h"

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

    QDoubleSpinBox* sensorValues[3];
    sensorValues[0] = ui->doubleSpinBox_1;
    sensorValues[1] = ui->doubleSpinBox_2;
    sensorValues[2] = ui->doubleSpinBox_3;

    QLabel* sensorLabels[3];
    sensorLabels[0] = ui->label_sensor_1;
    sensorLabels[1] = ui->label_sensor_2;
    sensorLabels[2] = ui->label_sensor_3;

    dbManager = *(new DBManager());
    dbManager.connect();

    dbManager.run_query(QUERY_SETTINGS);

    Setting setting;

    if (dbManager.hasNext()) {
        setting = dbManager.getSetting();
    }

    ui->lineEdit_host_name->setText(setting.brokerName);
    ui->lineEdit_broker_port->setText(QString::number(setting.brokerPort));

    dbManager.run_query(QUERY_TOPICS);

    Topic topic;

    while (dbManager.hasNext()) {
        topic = dbManager.getTopic();
        ui->listWidget_topic_list->addItem(topic.name);
    }

    dbManager.run_query(QUERY_OPERATORS);

    Operator op;

    while (dbManager.hasNext()) {
        op = dbManager.getOperator();
        ui->comboBox_user->addItem(op.fullname);
    }

    dbManager.run_query(QUERY_SENSORS);

    Sensor sensor;

    int counter = 0;
    while (dbManager.hasNext()) {

        sensor = dbManager.getSensor();

        sensorValues[counter]->setRange(sensor.minVal, sensor.maxVal);
        sensorLabels[counter]->setText(sensor.name);

        counter++;
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

   JSONFormatter jsonFormatter;

   time_t now = time(0);
   char* dt = ctime(&now);
   *(dt+24) = '\0';

   dbManager.run_query(QUERY_SETTINGS);

   Setting setting;

   if (dbManager.hasNext()) {
       setting = dbManager.getSetting();
   }

   QString message;


   jsonFormatter.insertLabel("product");
   jsonFormatter.insertValue(setting.product);

   jsonFormatter.insertLabel("version");
   jsonFormatter.insertValue(setting.version);

   jsonFormatter.insertLabel("date");
   jsonFormatter.insertValue(dt);

   jsonFormatter.insertLabel("serial");
   jsonFormatter.insertValue(setting.serial);

   jsonFormatter.insertLabel("demo");
   jsonFormatter.insertValue(QString::number(setting.demo));

   QString operatorName = ui->comboBox_user->currentText();

   dbManager.run_query(DBManager::FX_QUERY_OPERATOR_BY_NAME(operatorName));

   Operator op;

   if (dbManager.hasNext()) {

        op = dbManager.getOperator();

   }

   dbManager.run_query(DBManager::FX_QUERY_EMAIL_BY_ID(op.emailId));

   Email email;

   if (dbManager.hasNext()) {
        email = dbManager.getEmail();
   }

   jsonFormatter.insertLabel("operator");

   jsonFormatter.openMultipleValue();

   jsonFormatter.insertLabel("id");
   jsonFormatter.insertValue(QString::number(op.id));

   jsonFormatter.insertLabel("name");
   jsonFormatter.insertValue(op.fullname);

   jsonFormatter.insertLabel("phone");
   jsonFormatter.insertValue(op.phone);

   jsonFormatter.insertLabel("email");

   jsonFormatter.openArray();
   jsonFormatter.insertValue(email.email);
   jsonFormatter.closeArray();

   jsonFormatter.closeMultipleValue();

   QString lab_sn1 = ui->label_sensor_1->text();
   QString lab_sn2 = ui->label_sensor_2->text();
   QString lab_sn3 = ui->label_sensor_3->text();

   double lab_sn1out = ui->doubleSpinBox_1->value();
   double lab_sn2out = ui->doubleSpinBox_2->value();
   double lab_sn3out = ui->doubleSpinBox_3->value();

   jsonFormatter.insertLabel("sensor");
   jsonFormatter.openArray();

   jsonFormatter.openMultipleValue();
   jsonFormatter.insertLabel(lab_sn1);
   jsonFormatter.insertValue(QString::number(lab_sn1out));
   jsonFormatter.closeMultipleValue();

   jsonFormatter.openMultipleValue();
   jsonFormatter.insertLabel(lab_sn2);
   jsonFormatter.insertValue(QString::number(lab_sn2out));
   jsonFormatter.closeMultipleValue();

   jsonFormatter.openMultipleValue();
   jsonFormatter.insertLabel(lab_sn3);
   jsonFormatter.insertValue(QString::number(lab_sn3out));
   jsonFormatter.closeMultipleValue();

   jsonFormatter.closeArray();

   qDebug().noquote() << "~~~~~~~~~~~~~~\n" << jsonFormatter.getMessage();

   return jsonFormatter.getMessage();
}

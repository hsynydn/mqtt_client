#include "mainwindow.h"

#include <string>
#include <QtCore>

#include <ctime>
#include <ctype.h>

#include <stdio.h>

#define QOS         1
#define TIMEOUT     10000L

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->lineEdit_host_name->setText("broker.hivemq.com");
    ui->lineEdit_broker_port->setText("1883");

    ui->listWidget_topic_list->addItem("metosoft/topic_1");
    ui->listWidget_topic_list->addItem("metosoft/topic_2");
    ui->listWidget_topic_list->addItem("metosoft/topic_3");

    ui->comboBox_user->addItem("Nihat Yalman");
    ui->comboBox_user->addItem("Hüseyin Aydın");
    ui->comboBox_user->addItem("Ayhan Demirhan");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_connect_clicked()
{

    getPayloadAsJson();

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
                      MQTTCLIENT_PERSISTENCE_NONE, NULL);

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
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    MQTTClient_publishMessage(mqttClient,
                              topic.toStdString().c_str(),
                              &pubmsg,
                              &token);

    printf("Waiting for up to %d seconds for publication of %s\n on topic %s for client with ClientID: %s\n",
           (int)(TIMEOUT/1000),
           payload.toStdString().c_str(),
           topic.toStdString().c_str(),
           client_id.toStdString().c_str());

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
   // convert now to string form
   char* dt = ctime(&now);
   *(dt+24) = '\0';

   QString message;
   message.insert(message.length(), "{\n");
   message.insert(message.length(), "\"product\": \"MQTT Test Application\",\n");
   message.insert(message.length(), "\"version\": 1.0,\n");
   message.insert(message.length(), "\"date\": \"");
   message.insert(message.length(), dt);
   message.insert(message.length(), "\",\n");
   message.insert(message.length(), "\"serial\": \"MS00013\",\n");
   message.insert(message.length(), "\"demo\": \"true\",\n");

   message.insert(message.length(), "\"operator\": {\n");
   message.insert(message.length(), "\"id\": 12345,\n");
   message.insert(message.length(), "\"name\": ");
   message.insert(message.length(), ui->comboBox_user->currentText());
   message.insert(message.length(), "\",\n");
   message.insert(message.length(), "\"phones\": \"90-123-45-678\",\n");
   message.insert(message.length(), "\"email\": [\n");
   message.insert(message.length(), "\"ny1@example.com\",\n");
   message.insert(message.length(), "\"ny2@example.com\"\n]\n},\n");

   message.insert(message.length(), "\"sensor\": [\n");

   message.insert(message.length(), "{\"moisture\" : ");
   message.insert(message.length(), ui->doubleSpinBox_moisture->text());
   message.insert(message.length(), "},\n");

   message.insert(message.length(), "{\"humidity\" : ");
   message.insert(message.length(), ui->doubleSpinBox_humidity->text());
   message.insert(message.length(), "},\n");

   message.insert(message.length(), "{\"temperature\" : ");
   message.insert(message.length(), ui->doubleSpinBox_temperature->text());
   message.insert(message.length(), "},\n]\n");

   message.insert(message.length(), "}");

   //qDebug().noquote() << message << "\n";

   ui->label_progress_feedback->setText(message);

   return message;
}

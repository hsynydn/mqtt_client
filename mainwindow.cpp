#include "mainwindow.h"
#include "mainwindow.h"

#include <string>

#include <QtCore>
#include <QtNetwork>

#define QOS         1
#define TIMEOUT     10000L

using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->editView_brokerAddress->setText("broker.hivemq.com");

    ui->listWidget_topicList->addItem("metosoft/topic_1");
    ui->listWidget_topicList->addItem("metosoft/topic_2");
    ui->listWidget_topicList->addItem("metosoft/topic_3");
}

void MainWindow::on_pushButton_connect_clicked()
{

    broker_address.clear();
    broker_address.insert(broker_address.length(), "tcp://");
    broker_address.insert(broker_address.length(), ui->editView_brokerAddress->text());
    broker_address.insert(broker_address.length(), ":");
    broker_address.insert(broker_address.length(), ui->spinBox_brokerPort->text());

    topic = "Maskarien";

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
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_send_clicked()
{

    int returnCode;

    topic = ui->label_currentTopic->text();

    QString payload = ui->payloadEdit->toPlainText();

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

void MainWindow::on_listWidget_topicList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    ui->label_currentTopic->setText(current->text());
}

void MainWindow::on_MainWindow_destroyed()
{
    MQTTClient_disconnect(mqttClient, 10000);
    MQTTClient_destroy(&mqttClient);
}

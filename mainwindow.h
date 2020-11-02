#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtNetwork>

#include "ui_mainwindow.h"
#include "MQTTClient.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    QString broker_address;
    QString client_id;
    QString topic;

    MQTTClient mqttClient;
    MQTTClient_connectOptions conn_opts;

    ~MainWindow();

private slots:
    void on_pushButton_connect_clicked();

    void on_pushButton_send_clicked();

    void on_listWidget_topicList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_MainWindow_destroyed();

private:
    Ui::MainWindow *ui;
    void connToHost(QString hostname);
    void initEnv();

    QTcpSocket *socket;
};
#endif // MAINWINDOW_H

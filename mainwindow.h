#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QMainWindow>

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
    void on_listWidget_topic_list_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_MainWindow_destroyed();
    QString getPayloadAsJson();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

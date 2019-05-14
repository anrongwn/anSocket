#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "tcpclient.h"
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
signals:
    void connectHost(const QString& host, const int port);

private slots:
    void on_pushButton_connect_clicked();

    void on_recvData(const QString & str);
    void on_clientError(const QString & erro);
    void on_connected(const int port);

    void on_pushButton_disconnect_clicked();

    void on_pushButton_send_clicked();
    void on_stateChanged(QAbstractSocket::SocketState socketState);

    void on_pushButton_network_clicked();

    void on_heartbeat_timeout();

private:
    Ui::MainWindow *ui;

    TcpClient client_;
    QTimer * heartbeat_timer_;    //HeartBeat
};

#endif // MAINWINDOW_H

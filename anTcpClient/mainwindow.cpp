#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRegExp>
#include <QRegExpValidator>
#include <QMessageBox>
#include <QDebug>
#include <QByteArray>
#include <QTime>
#include <QDateTime>
#include <QNetworkInterface>
#include <QList>
#include "antlv.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //
    heartbeat_timer_ = new QTimer(this);
    QObject::connect(heartbeat_timer_, &QTimer::timeout, this, &MainWindow::heartbeat);


    QObject::connect(this, &MainWindow::connectHost, &client_, &TcpClient::onConnectHost);
    QObject::connect(&client_, &TcpClient::recvData, this, &MainWindow::on_recvData);
    QObject::connect(&client_, &TcpClient::clientError, this, &MainWindow::on_clientError);
    QObject::connect(&client_, &TcpClient::connected, this, &MainWindow::on_connected);
    QObject::connect(&client_, QOverload<QAbstractSocket::SocketState>::of(&QAbstractSocket::stateChanged), this, &MainWindow::on_stateChanged);


    QRegExp rx("^((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)$");
    ui->lineEdit_ip->setValidator(new QRegExpValidator(rx, this));
    ui->lineEdit_ip->setText(QStringLiteral("127.0.0.1"));

    QRegExp rx2("[0-9]\\d{0,5}");
    ui->lineEdit_port->setValidator(new QRegExpValidator(rx2, this));
    ui->lineEdit_port->setText(QStringLiteral("9555"));

    ui->pushButton_disconnect->setDisabled(true);
    ui->pushButton_send->setDisabled(true);
    ui->textEdit_echo->setReadOnly(true);



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_connect_clicked()
{
    if (ui->lineEdit_ip->text().isEmpty()){
        QMessageBox::information(this, "anTcpClient", "please input ip addr!");
        return;
    }
    if (ui->lineEdit_port->text().isEmpty()){
        QMessageBox::information(this, "anTcpClient", "please input port!");
        return;
    }

    emit connectHost(ui->lineEdit_ip->text(), ui->lineEdit_port->text().toInt());

}

void MainWindow::on_recvData(const QString &str)
{
    ui->textEdit_echo->append(str);
}

void MainWindow::on_clientError(const QString &erro)
{
    ui->textEdit_echo->append(erro);
    //qDebug()<<"===MainWindow::on_clientError " << erro;
}

void MainWindow::on_connected(const int port)
{
    ui->textEdit_echo->append(QString("connect succeed, localport=%1").arg(port));
    //qDebug()<<"===MainWindow::on_connected(" << port << ")";
    ui->pushButton_connect->setDisabled(true);
    ui->pushButton_disconnect->setEnabled(true);
    ui->pushButton_send->setEnabled(true);

    //启动心跳检测
    heartbeat_timer_->start(3000);
}

void MainWindow::on_pushButton_disconnect_clicked()
{
    client_.disconnectFromHost();
    if ((QAbstractSocket::UnconnectedState==client_.state())||(client_.waitForDisconnected(5000))){

        ui->textEdit_echo->append("disconnect succeed.");
        ui->pushButton_connect->setEnabled(true);
        ui->pushButton_disconnect->setEnabled(false);
        ui->pushButton_send->setEnabled(false);

        //
        heartbeat_timer_->stop();

    }

}

void MainWindow::on_pushButton_send_clicked()
{
    if (ui->textEdit_data->toPlainText().isEmpty()){
        QMessageBox::information(this, "anTcpClient", "please input send data.");
        return;
    }

    QString data = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    data+=" >>>> ";
    data+=ui->textEdit_data->toPlainText();

    QByteArray cmd = antlv::make_cmd_package(data.toLocal8Bit(), data.length());

    client_.write(cmd);
    //client_.write(data.toLocal8Bit());
    if (client_.waitForBytesWritten(1000)){
        ui->textEdit_echo->append(QString("send data:%1 >>>succeed.").arg(data));
    }
    else {
        ui->textEdit_echo->append(QString("send data:1% >>>failed.").arg(data));
    }

    //antlv::free_raw_data(cmd);

}

void MainWindow::on_stateChanged(QAbstractSocket::SocketState socketState)
{
    ui->textEdit_echo->append(QString("socket state changed : %1").arg(socketState));
}

void MainWindow::on_pushButton_network_clicked()
{
    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
    bool isConnected = false;

    for (int i = 0; i < ifaces.count(); i++)
    {
        QNetworkInterface iface = ifaces.at(i);

        QString addr = iface.hardwareAddress();
        QString name = iface.humanReadableName();

        if ( iface.flags().testFlag(QNetworkInterface::IsUp)
             && iface.flags().testFlag(QNetworkInterface::IsRunning)
             && !iface.flags().testFlag(QNetworkInterface::IsLoopBack)
             )
        {

            // this loop is important
            for (int j=0; j<iface.addressEntries().count(); j++)
            {
                // we have an interface that is up, and has an ip address
                // therefore the link is present

                // we will only enable this check on first positive,
                // all later results are incorrect
                if (isConnected == false)
                    isConnected = true;
            }
        }

    }


}

void MainWindow::heartbeat()
{
    QByteArray cmd = antlv::make_heartbeat_package();

    client_.write(cmd);
    if (client_.waitForBytesWritten(1000)){
        //ui->textEdit_echo->append(QString("send data:%1 >>>succeed.").arg(data));

        /*
        QByteArray resp = client_.readAll();
        if (!client_.waitForReadyRead(3000)){
            ui->textEdit_echo->append(QString("recv heartbeat package failed."));
        }
        */

    }
    else {
        ui->textEdit_echo->append(QString("send heartbeat package failed."));
    }

    //antlv::free_raw_data(cmd);


}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRegExp>
#include <QRegExpValidator>
#include <QMessageBox>
#include <QDebug>
#include <QByteArray>
#include <QTime>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
}

void MainWindow::on_pushButton_disconnect_clicked()
{
    client_.disconnectFromHost();
    if ((QAbstractSocket::UnconnectedState==client_.state())||(client_.waitForDisconnected(5000))){

        ui->textEdit_echo->append("disconnect succeed.");
        ui->pushButton_connect->setEnabled(true);
        ui->pushButton_disconnect->setEnabled(false);
        ui->pushButton_send->setEnabled(false);

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

    client_.write(data.toLocal8Bit());
    if (client_.waitForBytesWritten(1000)){
        ui->textEdit_echo->append(QString("send data:%1 >>>succeed.").arg(data));
    }
    else {
        ui->textEdit_echo->append(QString("send data:1% >>>failed.").arg(data));
    }

}

void MainWindow::on_stateChanged(QAbstractSocket::SocketState socketState)
{
    ui->textEdit_echo->append(QString("socket state changed : %1").arg(socketState));
}

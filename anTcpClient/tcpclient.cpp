#include "tcpclient.h"

TcpClient::TcpClient(QObject *parent):QTcpSocket(parent)
{
    QObject::connect(this, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &TcpClient::onError);
    QObject::connect(this, &QTcpSocket::readyRead, this, &TcpClient::onReadData);
}

TcpClient::~TcpClient()
{
}

void TcpClient::onError(QAbstractSocket::SocketError socketError)
{
    QString str(QString::number(this->peerPort()));
    str += " : erro : ";
    str += this->errorString();

    emit clientError(str);
}

void TcpClient::onReadData()
{
    QString str(QString::number(this->peerPort()));
    str += " : ";
    str += this->readAll();

    emit this->recvData(str);
}

void TcpClient::onConnectHost(const QString &host, const int port)
{
    this->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    this->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    this->connectToHost(host, port);

    if (!this->waitForConnected(5000)){
        //emit clientError(errorString());
    }
    else{
        emit connected(this->localPort());
    }
}


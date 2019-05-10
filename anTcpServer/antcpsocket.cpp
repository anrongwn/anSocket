#include "antcpsocket.h"
#include <QElapsedTimer>
#include <QTime>
#include <QDateTime>
#include <QHostAddress>

anTcpSocket::anTcpSocket(QObject *parent):QTcpSocket(parent), socketDescriptor_(0)
{
    QObject::connect(this, &QTcpSocket::readyRead, this, &anTcpSocket::onReadData);
    QObject::connect(this, &QTcpSocket::disconnected, this, &anTcpSocket::onDisconnected);

}

anTcpSocket::~anTcpSocket()
{

}

void anTcpSocket::onReadData()
{
    auto data  = handler(this->readAll(), this->peerAddress().toString(), this->peerPort());

    this->write(data);
}

void anTcpSocket::onDisconnected()
{

}

void anTcpSocket::onSentData(const QByteArray &data, const qintptr id)
{
    if (id == socketDescriptor_){
        this->write(data);
    }
}

void anTcpSocket::onDisConTcp(const qintptr id)
{
    if (id == socketDescriptor_){
        this->disconnectFromHost();
    }else if(-1==id){
        QObject::disconnect(this, &QTcpSocket::disconnected, this, &anTcpSocket::onDisconnected);
        this->disconnectFromHost();
        this->deleteLater();
    }
}

QByteArray anTcpSocket::handler(QByteArray data, const QString &ip, const qint16 port)
{

    QElapsedTimer tm;
    tm.start();

    while(tm.elapsed() < 1){

    }

    data = "<<<< " + ip.toLocal8Bit() + " " + QByteArray::number(port) + " " + data + " " +  QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ").toLocal8Bit();
    return data;
}

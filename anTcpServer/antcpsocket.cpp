#include "antcpsocket.h"
#include <QElapsedTimer>
#include <QTime>
#include <QDateTime>
#include <QHostAddress>
#include <QThread>
#include <QString>
#include <QTextStream>
#include <QDebug>
#include <QByteArray>
#include "antlv.h"

anTcpSocket::anTcpSocket(QObject *parent):QTcpSocket(parent), socketDescriptor_(0)
{
    QObject::connect(this, &QTcpSocket::readyRead, this, &anTcpSocket::onReadData);
    QObject::connect(this, &QTcpSocket::disconnected, this, &anTcpSocket::onDisconnected);

}

anTcpSocket::~anTcpSocket()
{
}

bool anTcpSocket::setSocketDescriptor(qintptr socketDescriptor, QAbstractSocket::SocketState socketState, QIODevice::OpenMode openMode)
{
    socketDescriptor_=socketDescriptor;
    return QTcpSocket::setSocketDescriptor(socketDescriptor, socketState, openMode);
}

void anTcpSocket::onReadData()
{
    QString logdata;
    QTextStream log(&logdata);

    log<<"anTcpSocket::onReadData(), tcp="<<this;

    datas_ += this->readAll();

    QByteArray data;
    antlv::antlv_type type = antlv::parse_package(datas_, data);
    switch (type){
    case antlv::package_type::heart_beat:
        log<<", recv heartbeat package,";
        break;
    case antlv::package_type::cmd_requst:
        log <<", recv data: "<<data;
        data  = handler(data, this->peerAddress().toString(), this->peerPort());
        break;
    case antlv::package_type::unknow:
    default:
        log <<", recv unknow package. ";
        break;
    }

    if (!data.isEmpty())
    {
        this->write(data);
        bool r = this->waitForBytesWritten(1000);
        if (r){
            log<<", echo write:" << data;
        }
        else{
            log<<", echo write failed.";
        }
    }
    log<<",th="<<QThread::currentThread();

    qDebug().noquote() << logdata;
}

void anTcpSocket::onDisconnected()
{
    QString logdata;
    QTextStream log(&logdata);

    log<<"anTcpSocket::onDisconnected(),tcp="<< this<<",socketDescriptor="<<socketDescriptor_<<",th="<<QThread::currentThread();

    emit sockDisConnect(socketDescriptor_, this->peerAddress().toString(), this->peerPort(), QThread::currentThread());//发送断开连接的用户信息

    //有延迟delete disconnet's tcp 的风险
    this->deleteLater();

    qDebug().noquote() << logdata;
}

void anTcpSocket::onSentData(const QByteArray &data, const qintptr id)
{
    QString logdata;
    QTextStream log(&logdata);

    log<<"anTcpSocket::onSentData(),tcp"<<(void*)this<<",data="<<data<<", socketDescriptor"<<id<<",th="<<QThread::currentThread();

    if (id == socketDescriptor_){
        this->write(data);
        bool r = this->waitForBytesWritten(1000);
        if (r){
            log<<", write:"<<data;
        }
        else{
            log<<", write failed.";
        }

    }

    qDebug().noquote() << logdata;
}

void anTcpSocket::onDisConTcp(const qintptr id)
{
    QString logdata;
    QTextStream log(&logdata);

    log<<"anTcpSocket::onSentData(),tcp"<<(void*)this<<", socketDescriptor"<<id<<",th="<<QThread::currentThread();


    if (id == socketDescriptor_){
        this->disconnectFromHost();
    }else if(-1==id){
        QObject::disconnect(this, &QTcpSocket::disconnected, this, &anTcpSocket::onDisconnected);
        this->disconnectFromHost();
        this->deleteLater();
    }
    qDebug().noquote() << logdata;
}

QByteArray anTcpSocket::handler(QByteArray data, const QString &ip, const quint16 port)
{

    QElapsedTimer tm;
    tm.start();

    while(tm.elapsed() < 1){

    }

    data = "<<<< " + ip.toLocal8Bit() + " " + QByteArray::number(port) + " " + data + " " +  QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ").toLocal8Bit();
    return data;
}

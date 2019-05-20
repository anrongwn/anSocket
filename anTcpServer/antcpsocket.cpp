#include "antcpsocket.h"
#include <QElapsedTimer>
#include <QTime>
#include <QDateTime>
#include <QHostAddress>
#include <QThread>
#include <QFuture>
#include <QtConcurrent>
#include <QString>
#include <QTextStream>
#include <QDebug>
#include <QByteArray>
#include <QVector>

#include "antlv.h"


anTcpSocket::anTcpSocket(QObject *parent):QTcpSocket(parent), socketDescriptor_(0)
{
    QObject::connect(this, &QTcpSocket::readyRead, this, &anTcpSocket::onReadData);
    QObject::connect(this, &QTcpSocket::disconnected, this, &anTcpSocket::onDisconnected);

     QObject::connect(this, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &anTcpSocket::onError);

}

anTcpSocket::~anTcpSocket()
{
    QString logdata;
    QTextStream log(&logdata);

    log<<"anTcpSocket::~anTcpSocket(), tcp="<<(void*)this<<",th="<<QThread::currentThread();
    qDebug().noquote() << logdata;
}

bool anTcpSocket::setSocketDescriptor(qintptr socketDescriptor, QAbstractSocket::SocketState socketState, QIODevice::OpenMode openMode)
{
    socketDescriptor_=socketDescriptor;
    return QTcpSocket::setSocketDescriptor(socketDescriptor, socketState, openMode);
}

QVector<QByteArray> anTcpSocket::parse_run(anTcpSocket *handle){
    QVector<QByteArray> result;

    while (!handle->datas_.isEmpty()){

        QByteArray resp;
        antlv::parse_package(handle->datas_, resp);
        if (!resp.isEmpty()){
            result.append(resp);
        }
    }

    return result;
}

void anTcpSocket::onReadData()
{
    //QElapsedTimer tm;
    //tm.start();

    QString logdata;
    QTextStream log(&logdata);

    log<<"anTcpSocket::onReadData(), tcp="<<this;

    datas_ += this->readAll();


    //启用并行处理，减少响应包延迟处理。可能增加了多客户端时，连接及时响应的风险。
    QFuture<QVector<QByteArray> > result = QtConcurrent::run(QThreadPool::globalInstance(), parse_run, this);
    result.waitForFinished();

    QVector<QByteArray> respV = result.result();
    for(auto it = respV.begin();it!=respV.end();++it){
        QByteArray data(it->data(),it->length());
        data  = handler(data, this->peerAddress().toString(), this->peerPort());

        if (!data.isEmpty())
        {

            this->write(data);
            bool r = this->waitForBytesWritten(1000);

            if (r){
                //log<<", echo write:" << data;
            }
            else{
                log<<", echo write failed.";
                qDebug().noquote() << logdata;
            }
        }

    }
    //qDebug().noquote() << logdata;

    /*
    QByteArray data;
    antlv::antlv_type type = antlv::parse_package(datas_, data);


    switch (type){
    case antlv::package_type::heart_beat:
        //log<<", recv heartbeat package,";
        break;
    case antlv::package_type::cmd_requst:
        //log <<", recv data: "<<data;
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
            //log<<", echo write:" << data;
        }
        else{
            log<<", echo write failed.";
        }
    }

    log<<",parse_package elapsed="<<tm.elapsed();
    //数据异常时输出日志
    if (datas_.size())
    {
        log<<",datas = "<< datas_.size()<<",th="<<QThread::currentThread();

        qDebug().noquote() << logdata;
    }
    */

}

void anTcpSocket::onDisconnected()
{
    QString logdata;
    QTextStream log(&logdata);

    log<<"anTcpSocket::onDisconnected(),tcp="<< this<<",socketDescriptor="<<socketDescriptor_<<",th="<<QThread::currentThread();
    qDebug().noquote() << logdata;

    emit sockDisConnect(socketDescriptor_, this->peerAddress().toString(), this->peerPort(), QThread::currentThread());//发送断开连接的用户信息
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

void anTcpSocket::onEnd(const qintptr id)
{
    QString logdata;
    QTextStream log(&logdata);

    log<<"anTcpSocket::onEnd(),tcp="<<(void*)this<<", socketDescriptor="<<id<<",th="<<QThread::currentThread();

    if (id == socketDescriptor_){

        //有延迟delete disconnet's tcp 的风险(anTcpSocket 内存泄漏)
        this->deleteLater();

    }else if(-1==id){
        //断开所有信号连接
        QObject::disconnect(this, nullptr);

        //QObject::disconnect(this, &QTcpSocket::disconnected, this, &anTcpSocket::onDisconnected);
        this->disconnectFromHost();

        //有延迟delete disconnet's tcp 的风险(anTcpSocket 内存泄漏)
        this->deleteLater();
    }



    qDebug().noquote() << logdata;
}

void anTcpSocket::onError(QAbstractSocket::SocketError socketError)
{
    QString logdata;
    QTextStream log(&logdata);

    log<<"anTcpSocket::onError("<<socketError<<"), socketDescriptor=" << socketDescriptor_ <<",errorstring="<<this->errorString()<<",th="<<QThread::currentThread();


    /*//
    emit sockDisConnect(socketDescriptor_, this->peerAddress().toString(), this->peerPort(), QThread::currentThread());//发送断开连接的用户信息
    */
    /*
    if (QAbstractSocket::RemoteHostClosedError==socketError){
        this->disconnectFromHost();
        if (QAbstractSocket::UnconnectedState!=this->state())
            this->waitForDisconnected(1000);
    }
    */

    qDebug().noquote() << logdata;
}

QByteArray anTcpSocket::handler(QByteArray data, const QString &ip, const quint16 port)
{

    /*//模拟 占用cpu
    QElapsedTimer tm;
    tm.start();

    while(tm.elapsed() < 1){

    }
    */

    data = "<<<< " + ip.toLocal8Bit() + " " + QByteArray::number(port) + " " + data + " " +  QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ").toLocal8Bit();
    return data;
}

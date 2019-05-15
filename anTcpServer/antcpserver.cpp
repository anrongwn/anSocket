#include "antcpserver.h"
#include "antcpsocket.h"
#include "anthreadpool.h"
#include <QHostAddress>
#include <QString>
#include <QTextStream>

anTcpServer::anTcpServer(QObject *parent, int maxConnections):QTcpServer(parent), maxConnections_(maxConnections)
{

}

anTcpServer::~anTcpServer()
{
    emit this->sentDisConnect(-1);
}

void anTcpServer::setMaxPendingConnections(int numConnections)
{
    this->maxConnections_=numConnections;
    this->QTcpServer::setMaxPendingConnections(numConnections);
}

void anTcpServer::onClear()
{
    QString logdata;
    QTextStream log(&logdata);

    log<<"anTcpServer::onClear().";

    emit this->sentDisConnect(-1);
    anThreadPool::instance().clear();
    client_.clear();

    qDebug().noquote()<<logdata;
}

void anTcpServer::onSockDisConnect(int handle, const QString &ip, quint16 port, QThread *th)
{
    QString logdata;
    QTextStream log(&logdata);

    log<<"anTcpServer::onSockDisConnect(socketDescriptor="<<handle<<",ip="<<ip<<",port="<<port<<",th="<<th<<").";

    client_.remove(handle);//连接管理中移除断开连接的socket

    anThreadPool::instance().removeThread(th); //告诉线程管理类那个线程里的连接断开了

    qDebug().noquote()<<logdata;

    //通知tcpsocket 做清理
    emit sentDisConnect(handle);
}

void anTcpServer::incomingConnection(qintptr socketDescriptor)
{
    QString logdata;
    QTextStream log(&logdata);

    log<<"anTcpServer::incomingConnection("<<socketDescriptor<<")";
    if (client_.size() > maxPendingConnections())//继承重写此函数后，QTcpServer默认的判断最大连接数失效，自己实现
       {
           QTcpSocket tcp;
           tcp.setSocketDescriptor(socketDescriptor);
           tcp.disconnectFromHost();

           log<<"client num("<<client_.size()<<") > maxPendingConnections("<<maxPendingConnections()<<").";
           qDebug().noquote()<<logdata;

           return;
       }

       auto th = anThreadPool::instance().getThread();
       auto tcpTemp = new anTcpSocket(nullptr);
       tcpTemp->setSocketDescriptor(socketDescriptor);
       tcpTemp->setSocketOption(QAbstractSocket::LowDelayOption, 1);
       tcpTemp->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

       QString ip =  tcpTemp->peerAddress().toString();
       quint16 port = tcpTemp->peerPort();

       connect(tcpTemp,&anTcpSocket::sockDisConnect, this,&anTcpServer::onSockDisConnect);//NOTE:断开连接的处理，从列表移除，并释放断开的Tcpsocket，此槽必须实现，线程管理计数也是考的他
       connect(this,&anTcpServer::sentDisConnect,tcpTemp,&anTcpSocket::onDisConTcp);//断开信号

       tcpTemp->moveToThread(th);
       client_.insert(socketDescriptor, tcpTemp);

       log<<", th="<<th<<",tcp="<<tcpTemp<<", socketDescriptor="<<socketDescriptor<<",ip="<<ip<<",port="<<port;

       emit connectClient(socketDescriptor,ip,port);

       qDebug().noquote()<<logdata;
}

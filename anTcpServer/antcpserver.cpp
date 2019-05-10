#include "antcpserver.h"
#include "antcpsocket.h"
#include "anthreadpool.h"
#include <QHostAddress>

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
    emit this->sentDisConnect(-1);
    anThreadPool::instance().clear();
    client_.clear();
}

void anTcpServer::onSockDisConnect(int handle, const QString &ip, quint16 prot, QThread *th)
{
    client_.remove(handle);//连接管理中移除断开连接的socket
    anThreadPool::instance().removeThread(th); //告诉线程管理类那个线程里的连接断开了

    emit sockDisConnect(handle,ip,prot);
}

void anTcpServer::incomingConnection(qintptr socketDescriptor)
{
    if (client_.size() > maxPendingConnections())//继承重写此函数后，QTcpServer默认的判断最大连接数失效，自己实现
       {
           QTcpSocket tcp;
           tcp.setSocketDescriptor(socketDescriptor);
           tcp.disconnectFromHost();
           return;
       }

       auto th = anThreadPool::instance().getThread();
       auto tcpTemp = new anTcpSocket(nullptr);
       tcpTemp->setSocketDescriptor(socketDescriptor);
       tcpTemp->setSocketOption(QAbstractSocket::LowDelayOption, 1);
       tcpTemp->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

       QString ip =  tcpTemp->peerAddress().toString();
       qint16 port = tcpTemp->peerPort();

       connect(tcpTemp,&anTcpSocket::sockDisConnect, this,&anTcpServer::onSockDisConnect);//NOTE:断开连接的处理，从列表移除，并释放断开的Tcpsocket，此槽必须实现，线程管理计数也是考的他
       connect(this,&anTcpServer::sentDisConnect,tcpTemp,&anTcpSocket::onDisConTcp);//断开信号

       tcpTemp->moveToThread(th);
       client_.insert(socketDescriptor, tcpTemp);
       emit connectClient(socketDescriptor,ip,port);
}

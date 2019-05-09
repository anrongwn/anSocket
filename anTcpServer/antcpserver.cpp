#include "antcpserver.h"
#include "antcpsocket.h"

anTcpServer::anTcpServer(QObject *parent, int maxConnections):QTcpServer(parent), maxConnections_(maxConnections)
{

}

anTcpServer::~anTcpServer()
{

}

void anTcpServer::setMaxPendingConnections(int numConnections)
{
    this->maxConnections_=numConnections;
    this->QTcpServer::setMaxPendingConnections(numConnections);
}

void anTcpServer::onClear()
{

}

void anTcpServer::onSockDisConnect(int handle, const QString &ip, quint16 prot, QThread *th)
{

}

void anTcpServer::incomingConnection(qintptr socketDescriptor)
{

}

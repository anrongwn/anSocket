#include "antcpsocket.h"

anTcpSocket::anTcpSocket(QObject *parent):QTcpSocket(parent), socketDescriptor_(0)
{

}

anTcpSocket::~anTcpSocket()
{

}

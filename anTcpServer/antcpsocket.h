#ifndef ANTCPSOCKET_H
#define ANTCPSOCKET_H
#include <QTcpSocket>

class anTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit anTcpSocket(QObject* parent);
    ~anTcpSocket();
private:
    qintptr socketDescriptor_;
};

#endif // ANTCPSOCKET_H

#ifndef TCPCLIENT_H
#define TCPCLIENT_H
#include <QTcpSocket>
#include <QTime>

class TcpClient : public QTcpSocket
{
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent=nullptr);
    ~TcpClient();

signals:
    void recvData(const QString & str);
    void clientError(const QString & erro);
    void connected(const int port);

public slots:
    void onError(QAbstractSocket::SocketError socketError);
    void onReadData();
    void onConnectHost(const QString& host, const int port);
};

#endif // TCPCLIENT_H

#ifndef ANTCPSERVER_H
#define ANTCPSERVER_H
#include <QTcpServer>
#include <QHash>

class anTcpSocket;
class anTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit anTcpServer(QObject* parent=nullptr, int maxConnections=1000);
    ~anTcpServer();

    void setMaxPendingConnections(int numConnections);

signals:
    void connectClient(const int , const QString & ,const quint16 );//发送新用户连接信息
    void readData(const int,const QString &, quint16, const QByteArray &);//发送获得用户发过来的数据
    void sockDisConnect(int ,QString ,quint16);//断开连接的用户信息
    void sentData(const QByteArray &,const int);//向scoket发送消息
    void sentDisConnect(int i); //断开特定连接，并释放资源，-1为断开所有。

public slots:
    void onClear(); //断开所有连接，线程计数器请0

protected slots:
    void onSockDisConnect(int handle,const QString & ip, quint16 prot, QThread *th);//断开连接的用户信息

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:
    int maxConnections_;
    QHash<qintptr, anTcpSocket*> client_;
};

#endif // ANTCPSERVER_H

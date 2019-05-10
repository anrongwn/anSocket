#ifndef ANTCPSOCKET_H
#define ANTCPSOCKET_H
#include <QTcpSocket>
#include <QQueue>
#include <QByteArray>
#include <QDataStream>

class anTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit anTcpSocket(QObject* parent);
    ~anTcpSocket();
signals:
    void sockDisConnect(const int ,const QString &,const quint16, QThread *);//通知服务端线程池断开连接计数

public slots:
    void onReadData();
    void onDisconnected();

    void onSentData(const QByteArray& data ,const qintptr id);//发送信号的槽
    void onDisConTcp(const qintptr id);

private:
    QByteArray handler(QByteArray data, const QString &ip, const qint16 port);
private:
    qintptr socketDescriptor_;
    QQueue<QByteArray> datas_;

};

#endif // ANTCPSOCKET_H

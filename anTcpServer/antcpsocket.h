#ifndef ANTCPSOCKET_H
#define ANTCPSOCKET_H
#include <QTcpSocket>
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

public:
    bool setSocketDescriptor(qintptr socketDescriptor, QAbstractSocket::SocketState socketState = ConnectedState, QIODevice::OpenMode openMode = ReadWrite) override;
public slots:
    void onReadData();
    void onDisconnected();

    void onSentData(const QByteArray& data ,const qintptr id);//发送信号的槽
    void onEnd(const qintptr id);

    void onError(QAbstractSocket::SocketError socketError);

private:
    QByteArray handler(QByteArray data, const QString &ip, const quint16 port);
    static QVector<QByteArray> parse_run(anTcpSocket *handle);
private:
    qintptr socketDescriptor_;
    QByteArray datas_;

};

#endif // ANTCPSOCKET_H

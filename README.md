# anSocket
利用QTcpServer、QTcpSocket 开发的多线程并发的socket server。


#anTcpClient
采用 QTcpSocket ，QTimer 定时轮巡 同步收发机制。


#anTcpServer
采用自制线程池处理 QTcpServer 的incomingConnection 发来的socket 连接，并在线程池
使用QtConcurrent::run 并行处理收发数据。

#测试说明
客户端测试：
局域内 同一PC上开启 10 个客户端，每客户端隔10ms 发送 1091 byte 数据，并同步接收1091 byte 数据。
配置：4G，i5

服务端表现：
CPU占用： 约8% 
内存占用：16.62M
服务线程：11个
配置：4G，i5
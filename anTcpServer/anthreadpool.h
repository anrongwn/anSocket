#ifndef ANTHREADPOOL_H
#define ANTHREADPOOL_H
#include <QThread>
#include <QHash>
#include <QString>
#include <QTcpSocket>
#include <QTcpServer>
#include <atomic>

class anThreadPool
{
public:
    ~anThreadPool();

private:
    anThreadPool();
    void initThreadPool(const unsigned int size);


public:
    static anThreadPool& instance();

    QThread* getThread();
    void removeThread(QThread *th);

private:
    unsigned int size_;
    std::atomic_bool init_flag_;
    QHash<QThread *, unsigned int> thread_pool_;


};

#endif // ANTHREADPOOL_H

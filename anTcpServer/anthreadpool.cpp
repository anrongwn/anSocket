#include "anthreadpool.h"
#include <algorithm>
#include <string>
#include <map>
#include <QTextStream>
#include <QDebug>

anThreadPool::~anThreadPool()
{
    QThread * tmp=nullptr;
    for(auto it = thread_pool_.begin();it!=thread_pool_.end();++it){
        tmp = it.key();
        tmp->exit(0);
        tmp->wait(3000);
        delete tmp;
        tmp=nullptr;
    }
    thread_pool_.clear();
}

anThreadPool::anThreadPool() : size_(0), init_flag_(false)
{

}

void anThreadPool::initThreadPool(const unsigned int size)
{
    if (init_flag_) return;

    QString logdata;
    QTextStream log(&logdata);

    size_ = (size>0?size:QThread::idealThreadCount());
    init_flag_=true;

    log<<"anThreadPool::initThreadPool("<<size_<<")";
    for(unsigned int i=0;i<size_;++i){
        QThread * th = new QThread(nullptr);
        thread_pool_.insert(th, 0);
        th->start();

        log<<",th="<<th;
    }

    qDebug().noquote()<<logdata;
}

anThreadPool &anThreadPool::instance()
{
    static anThreadPool tp;
    return tp;
}

QThread *anThreadPool::getThread()
{
    if (!init_flag_){
        initThreadPool(QThread::idealThreadCount());
    }

    QString logdata;
    QTextStream log(&logdata);

    log<<"anThreadPool::getThread()";
    auto nit = thread_pool_.begin();
    for(auto it=thread_pool_.begin();it!=thread_pool_.end();++it){
        if (it.value() < nit.value()){
            nit = it;
        }
    }

    nit.value()++;

    log<<",th="<<nit.key()<<",value="<<nit.value();

    qDebug().noquote().noquote()<<logdata;

    return nit.key();
}

void anThreadPool::removeThread(QThread *th)
{
    QString logdata;
    QTextStream log(&logdata);

    log<<"anThreadPool::removeThread(th="<<th<<"),value=";

    auto it = thread_pool_.find(th);
    if (it!=thread_pool_.end()){

        --it.value();
        /*//
        if(0==(it.value())){
            thread_pool_.remove(th);

            th->exit(0);
            th->wait(3000);
            delete th;
        }
        */

        log<<it.value();
    }
    qDebug().noquote()<<logdata;
}

void anThreadPool::clear()
{
    for(auto it = thread_pool_.begin();it!=thread_pool_.end();++it){
        it.value()=0;
    }
}

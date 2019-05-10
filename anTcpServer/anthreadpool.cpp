#include "anthreadpool.h"
#include <algorithm>
#include <string>
#include <map>

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

    size_ = (size>0?size:QThread::idealThreadCount());
    init_flag_=true;

    for(unsigned int i=0;i<size_;++i){
        QThread * th = new QThread(nullptr);
        thread_pool_.insert(th, 0);
        th->start();
    }
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

    auto nit = thread_pool_.begin();
    for(auto it=thread_pool_.begin();it!=thread_pool_.end();++it){
        if (it.value() < nit.value()){
            nit = it;
        }
    }

    nit.value()++;
    return nit.key();
}

void anThreadPool::removeThread(QThread *th)
{
    auto it = thread_pool_.find(th);
    if (it!=thread_pool_.end()){

        --it.value();
        /*
        if(0==(it.value())){
            thread_pool_.remove(th);

            th->exit(0);
            th->wait(3000);
            delete th;
        }
        */
    }
}

void anThreadPool::clear()
{
    for(auto it = thread_pool_.begin();it!=thread_pool_.end();++it){
        it.value()=0;
    }
}

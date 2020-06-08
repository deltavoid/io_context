#ifndef IO_CONTEXT_H
#define IO_CONTEXT_H

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>


#include "blocking_queue.hpp"


typedef std::function<void()> Func;

class IOContext;


class EpollHandler
{public:
    virtual int handle(uint32_t events) { return -1;}
    virtual ~EpollHandler() {}
};


class NotifyHandler : public EpollHandler
{public:
    int notify_fd;
    BlockingQueue<Func> notify_queue;
    IOContext* io_context;

    NotifyHandler(IOContext* io_context);

    virtual ~NotifyHandler();

    virtual int handle(uint32_t events);
};


class IOContext
{public:
    int epoll_fd;
    
    NotifyHandler notify_handler;

    std::queue<Func> internal_queue;

    IOContext();

    ~IOContext();


};




#endif
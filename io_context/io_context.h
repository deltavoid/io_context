#ifndef IO_CONTEXT_H
#define IO_CONTEXT_H

#include <queue>
#include <thread>
#include <mutex>
#include <functional>





typedef std::function<void()> Func;

class IOContext;


class EpollHandler
{public:
    virtual int handle(uint32_t events) { return -1;}
    virtual ~EpollHandler() {}
};


class NotifyHandler : public EpollHandler
{public:
    int notify_fd; // event_fd
    std::queue<Func> notify_queue;
    std::mutex mutex;
    IOContext* io_context;

    NotifyHandler(IOContext* io_context);

    virtual ~NotifyHandler();

    virtual int handle(uint32_t events);

    void put(Func func);
    
    void notify();
    void acknowledge();
    
};


class TimeHandler;

class IOContext
{public:
    int epoll_fd;
    bool running;    
    NotifyHandler* notify_handler;
    TimeHandler* time_handler;
    std::queue<Func> internal_queue;

    IOContext();

    ~IOContext();

    void run();

    void put(Func func);

};




#endif

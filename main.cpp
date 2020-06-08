#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

#include <unistd.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <pthread.h>


typedef std::function<void()> Func;


template<typename Elem>
class BlockingQueue
{public:
    std::queue<Elem> queue;
    std::mutex mutex;
    std::condition_variable not_empty;

    BlockingQueue()
    {
    }

    ~BlockingQueue()
    {
    }

    void put(Elem x)
    {
        std::lock_guard<std::mutex> guard(mutex);
        queue.push(x);
        not_empty.notify_one();
    }

    Elem take()
    {
        std::unique_lock<std::mutex> lock(mutex);
        not_empty.wait(lock, [this] { return !queue.empty();});
        
        Elem x = queue.front();
        queue.pop();

        return x;
    }

    bool empty()
    {
        std::lock_guard<std::mutex> guard(mutex);
        return queue.empty();
    }

};


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

    NotifyHandler(IOContext* io_context) : io_context(io_context)
    {
    }

    virtual ~NotifyHandler()
    {
    }

    virtual int handle(uint32_t events)
    {
        return 0;
    }
};


class IOContext
{public:
    int epoll_fd;
    
    NotifyHandler notify_handler;

    std::queue<Func> internal_queue;

    IOContext() : notify_handler(this)
    {
    }

    ~IOContext()
    {
    }






};





int main()
{
    printf("hello world\n");
}
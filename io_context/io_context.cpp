#include "io_context.h"
#include "timer.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <pthread.h>


NotifyHandler::NotifyHandler(IOContext* io_context)
    : io_context(io_context)
{
    notify_fd = eventfd(0, 0);
    
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.ptr = this;
    if  (epoll_ctl(io_context->epoll_fd, EPOLL_CTL_ADD, notify_fd, &ev) < 0)
    {   perror("epoll_ctl_add notify_fd error");
    }

}


NotifyHandler::~NotifyHandler()
{
    if  (epoll_ctl(io_context->epoll_fd, EPOLL_CTL_DEL, notify_fd, NULL) < 0)
    {   perror("epoll_ctl_del notify_fd error");
    }

    close(notify_fd);
}


int NotifyHandler::handle(uint32_t events)
{
    acknowledge();
    
    {
        std::lock_guard<std::mutex> guard(mutex);
        while (!notify_queue.empty())
        {
            io_context->put(notify_queue.front());
            notify_queue.pop();
        }
    }
    
    return 0;
}

void NotifyHandler::put(Func func)
{
    {
        std::lock_guard<std::mutex> guard(mutex);
        notify_queue.push(func);
    }

    notify();
}

void NotifyHandler::notify()
{
    uint64_t val = 1;
    if  (write(notify_fd, &val, sizeof(val)) != sizeof(val))
    {   perror("write eventfd error");
    }
}

void NotifyHandler::acknowledge()
{
    uint64_t val = 0;
    if  (read(notify_fd, &val, sizeof(val)) != sizeof(val))
    {   perror("read eventfd error");
    }
}




IOContext::IOContext()
{
    epoll_fd = epoll_create1(0);
    notify_handler = new NotifyHandler(this);
    // time_handler = new TimeHandler(this);
    running = true;

}

IOContext::~IOContext()
{
    delete notify_handler;
    close(epoll_fd);
}


void IOContext::run()
{
    printf("IOContext::run: 1, thread: %lx\n", pthread_self());
    const int internal_events_num = 1024;
    struct epoll_event internal_events[internal_events_num];

    while (running)
    {
        printf("-----------------------------------------------------\n");
        printf("IOContext::run: 2, thread: %lx\n", pthread_self());
        int num = epoll_wait(epoll_fd, internal_events, internal_events_num, -1);
        if  (num < 0)
            perror("epoll_wait error");
        
        printf("IOContext::run: 3, thread: %lx\n", pthread_self());
        for (int i = 0; i < num; i++)
        {
            struct epoll_event& ev = internal_events[i];
            EpollHandler* handler = (EpollHandler*)ev.data.ptr;
            int ret = 0;

            if  ((ret = handler->handle(ev.events)) < 0)
            {
                printf("handler %lx return %d, to be deleted\n", 
                        (unsigned long)handler, ret);
                delete handler;
            }
        }

        printf("IOContext::run: 4, thread: %lx\n", pthread_self());
        while (!internal_queue.empty())
        {
            Func func = internal_queue.front();
            internal_queue.pop();

            func();
        }

        printf("IOContext::run: 5, thread: %lx\n", pthread_self());
    }

    printf("IOContext::run: 6, thread: %lx\n", pthread_self());
}

void IOContext::put(Func func)
{
    internal_queue.push(func);
}
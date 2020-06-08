#include "io_context.h"

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
    return 0;
}



IOContext::IOContext()
{
    epoll_fd = epoll_create1(0);
    running = true;

}

IOContext::~IOContext()
{
    close(epoll_fd);
}


void IOContext::run()
{
    printf("IOContext::run: 1\n");
    const int internal_events_num = 1024;
    struct epoll_event internal_events[internal_events_num];

    while (running)
    {
        printf("IOContext::run: 2\n");
        int num = epoll_wait(epoll_fd, internal_events, internal_events_num, -1);
        if  (num < 0)
            perror("epoll_wait error");
        
        printf("IOContext::run: 3\n");
        for (int i = 0; i < num; i++)
        {
            struct epoll_event& ev = internal_events[i];
            EpollHandler* handler = (EpollHandler*)ev.data.ptr;

            if  (handler->handle(ev.events) < 0)
                delete handler;
        }

        printf("IOContext::run: 4\n");
        while (!internal_queue.empty())
        {
            Func func = internal_queue.front();
            internal_queue.pop();

            func();
        }

        printf("IOContext::run: 5\n");
    }

    printf("IOContext::run: 6\n");
}
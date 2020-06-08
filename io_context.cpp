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
    
    struct epoll_event event = {
        .events = EPOLLIN,
        .data.ptr = this
    };
    if  (epoll_ctl(io_context->epoll_fd, EPOLL_CTL_ADD, notify_fd, &event) < 0)
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
}

IOContext::~IOContext()
{
}
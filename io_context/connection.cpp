#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#include "connection.h"
#include "ring_buffer.h"



Connection::Connection(IOContext* io_context, int fd)
    : io_context(io_context), fd(fd)
{
    printf("Connection::Connection: 1\n");
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.ptr = this;
    if  (epoll_ctl(io_context->epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0)
    {   perror("epoll_ctl_add conn_fd error");
    }

    printf("Connection::Connection: 2\n");
}

Connection::~Connection()
{
    printf("Connection::~Connection: 1\n");
    if  (epoll_ctl(io_context->epoll_fd, EPOLL_CTL_DEL, fd, NULL) < 0)
    {   perror("epoll_ctl_del conn_fd error");
    }

    close(fd);
    printf("Connection::~Connection: 2\n");
}

int Connection::handle(uint32_t events)
{
    printf("Connection::handle: 1, events: %d\n", events);
    if  (events & ~(EPOLLIN | EPOLLOUT))
        return -1;
    int ret = 0;

    printf("Connection::handle: 2\n");
    if  (events & EPOLLIN)
    {   ret = echo();
        if  (ret < 0)  return ret;
    }

    printf("Connection::handle: 3\n");
    return 0;
}

int Connection::echo()
{
    const int buf_size = 1024;
    char buf[buf_size];

    int recvd = ::recv(fd, buf, buf_size, 0);
    if  (recvd <= 0)  return -1;

    int sent = ::send(fd, buf, recvd, 0);
    if  (sent != recvd) return -1;
    return 0;
}
#include <errno.h>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "acceptor.h"
#include "connection.h"



Acceptor::Acceptor(IOContext* io_context, struct sockaddr_in* addr)
    : io_context(io_context)
{
    fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if  (fd < 0)
    {   perror("socket listen_fd error");
    }

    if  (bind(fd, (struct sockaddr*)addr, (socklen_t)sizeof(*addr)) < 0)
    {    perror("bind error");
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.ptr = this;
    if  (epoll_ctl(io_context->epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0)
    {   perror("epoll_ctl_add listen_fd error");
    }
}

Acceptor::~Acceptor()
{
    if  (epoll_ctl(io_context->epoll_fd, EPOLL_CTL_DEL, fd, NULL) < 0)
    {   perror("epoll_ctl_del listen_fd error");
    }

    close(fd);
}



int Acceptor::handle(uint32_t events)
{
    printf("Acceptor::handle: 1, events: 0x%x\n", events);
    if  (events & ~EPOLLIN)  return -1;
    printf("Acceptor::handle: 2\n");
    int ret = 0;

    if  (events & EPOLLIN)
    {   ret = accept();
        if  (ret < 0)  return -1;
    }
        
    return 0;
}

int Acceptor::listen(int backlog)
{
    return ::listen(fd, backlog);
}

int Acceptor::accept()
{
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    int newfd = ::accept4(fd, (struct sockaddr*)&addr, &addr_len, SOCK_NONBLOCK);
    if  (newfd < 0)
    {   if  (errno == EAGAIN || errno == EWOULDBLOCK)
            return 0;
        perror("accept errror");
        return -1;
    }

    // Conection* conn = new Connection(io_context);
    return 0;
}
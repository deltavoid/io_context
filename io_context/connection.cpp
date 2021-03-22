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

    could_read = false;
    could_write = true;


    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
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
    if  (events & EPOLLOUT)
    {   ret = handle_write();
        if  (ret < 0)  
            return ret;
    }

    printf("Connection::handle: 2\n");
    if  (events & EPOLLIN)
    {   ret = handle_read();
        if  (ret < 0)  
            return ret;
    }

    printf("Connection::handle: 3\n");
    return 0;
}

// int Connection::echo()
// {
//     const int buf_size = 1024;
//     char buf[buf_size];

//     int recvd = ::recv(fd, buf, buf_size, 0);
//     if  (recvd <= 0)  return -1;

//     int sent = ::send(fd, buf, recvd, 0);
//     if  (sent != recvd) return -1;
//     return 0;
// }


int Connection::handle_read()
{
    could_read = true;
    
    int recv_len = recv_buf();

    if  (recv_len < 0)
        return -1;

    return 0;
}

int Connection::handle_write()
{
    could_write = true;

    int send_len = send_buf();

    if  (send_len < 0)
        return -1;

    return 0;
}


int Connection::recv_buf()
{
    if  (!check_read())
        return 0;

    // at this stage, the buf is not full and fd is readable.

    int recv_len = 0, recv_total = 0;

    while ((recv_len = send_buf_once()) > 0)
    {
        recv_total += recv_len;
    }

    if  (recv_len < 0)
        return -1;

    if  (check_write())
        io_context->put( [this]() { this->send_buf();} );

    return recv_total;
}


// buf is not empty entry;
int Connection::send_buf()
{
    if  (!check_write())
        return 0;

    // at this stage, buf is not empty and fd is writable.

    int send_len = 0, send_total = 0;
    while ((send_len = send_buf_once()) > 0)
    {
        send_total += send_len;
    }

    if  (send_len < 0)
        return -1;

    if  (check_read())
        io_context->put( [this]() { this->recv_buf(); } );

    return send_total;
}



// assume buf is not full and fd is readable.
int Connection::recv_buf_once()
{
    if  (!check_read())
        return 0;

    int to_read = buf.get_blank_size();

        struct iovec iov[2];
        size_t iov_len = 0;
        if  (buf.get_blank_iovec(iov, &iov_len) < 0)
            return 0;



        struct msghdr msg = {
            .msg_name = NULL,
            .msg_namelen = 0,
            .msg_iov = iov,
            .msg_iovlen = iov_len,
            .msg_control = NULL,
            .msg_controllen = 0
        };
        int recv_len = recvmsg(fd, &msg, 0);
        printf("Connection::recv_buf, recv_len: %d\n", recv_len);


        if  (recv_len == 0)
        {   return -1;
        }
        else if  (recv_len < 0)
        {   if  (errno == EAGAIN || errno == EWOULDBLOCK)
                recv_len = 0;
            else
                return -1;
        }

    buf.add_exist(recv_len);

    return recv_len;
}


// assume buf is not empty and fd is writable
// send once
int Connection::send_buf_once()
{
    if  (!check_write())
        return 0;

    int to_send = buf.get_exist_size();

    struct iovec iov[2];
    size_t iov_len = 0;
    buf.get_exist_iovec(iov, &iov_len);


    struct msghdr msg = {
            .msg_name = NULL,
            .msg_namelen = 0,
            .msg_iov = iov,
            .msg_iovlen = iov_len,
            .msg_control = NULL,
            .msg_controllen = 0
        };
        int send_len = sendmsg(fd, &msg, 0);
        printf("Connection::send_buf, send_len: %d\n", send_len);
                
    if  (send_len < 0)
        {
            if  (errno == EAGAIN || errno == EWOULDBLOCK)
            {   
                could_write = false;
                send_len = 0;
            }
            else
                return -1;
        }

    buf.add_blank(send_len);

    return send_len;
}
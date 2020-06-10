#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <unistd.h>
#include <sys/timerfd.h>
#include <sys/epoll.h>


#include "timer.h"





TimeHandler::TimeHandler(IOContext* io_context)
    : io_context(io_context)
{
    fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK);
    if  (fd < 0)
    {   perror("timerfd_create error");
    }

    struct timespec now;
    if (clock_gettime(CLOCK_REALTIME, &now) < 0)
    {   perror("clock_gettime error");
    }

    int initial_expiration = 2;
    int interval = 1;
    // int total_times = 5;

    struct itimerspec itimerspec;
    itimerspec.it_value.tv_sec = now.tv_sec + initial_expiration;
    itimerspec.it_value.tv_nsec = now.tv_nsec;
    itimerspec.it_interval.tv_sec = interval;
    itimerspec.it_interval.tv_nsec = 0;

    if (timerfd_settime(fd, TFD_TIMER_ABSTIME, &itimerspec, NULL) < 0)
    {   perror("timerfd_settime error");
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.ptr = this;
    if  (epoll_ctl(io_context->epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0)
    {   perror("epoll_ctl_add timerfd error");
    }
}

TimeHandler::~TimeHandler()
{
    if  (epoll_ctl(io_context->epoll_fd, EPOLL_CTL_DEL, fd, NULL) < 0)
    {   perror("epoll_ctl_del timerfd error");
    }

    close(fd);
}

int TimeHandler::handle(uint32_t events)
{
    printf("TimeHandler::handle: 1, events: 0x%x, \n", events);
    printf("events&~EPOLLIN: %x, (events&~EPOLLIN) != 0: %d\n", 
            events & ~EPOLLIN, (events & ~EPOLLIN) != 0);

    if  (events & ~EPOLLIN) return -1;

    int ret = 0;
    if  (events & EPOLLIN)
    {   ret = handle_read();
        if  (ret < 0)  return ret;
    }

    return 0;
}

int TimeHandler::handle_read()
{
    // printf("TimeHandler::handle_read: 1\n");
    uint64_t num = read_timerfd();

    struct timespec now;
    if (clock_gettime(CLOCK_REALTIME, &now) < 0)
    {   perror("clock_gettime error");
    }

    printf("timespec: %lld, %lld, expire_times: %llu\n", 
            now.tv_sec, now.tv_nsec, num);

    return 0;
}

uint64_t TimeHandler::read_timerfd()
{
    uint64_t val;
    if  (read(fd, &val, sizeof(val)) != sizeof(val))
    {   perror("read timerfd error");
    }
    return val;
}
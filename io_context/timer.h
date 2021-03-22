#ifndef TIMER_H
#define TIMER_H

#include "io_context.h"

class Timer
{
};

class TimeHandler : public EpollHandler
{public:
    int fd; // timerfd
    IOContext* io_context;


    TimeHandler(IOContext* io_context);
    virtual ~TimeHandler();

    virtual int handle(uint32_t events);
    int handle_read();

    uint64_t read_timerfd();


};


#endif
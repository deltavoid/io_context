#ifndef CONNECTION_H
#define CONNECTION_H

#include "io_context.h"


class Connection : public EpollHandler
{public:
    int fd;
    IOContext* io_context;

    Connection(IOContext* io_context, int fd);
    virtual ~Connection();

    virtual int handle(uint32_t events);
    int echo();
};

#endif
#ifndef CONNECTION_H
#define CONNECTION_H

#include "io_context.h"
#include "ring_buffer.h"


class Connection : public EpollHandler
{public:
    int fd;
    
    bool could_read, could_write;

    IOContext* io_context;

    RingBuffer buf;


    Connection(IOContext* io_context, int fd);
    virtual ~Connection();

    virtual int handle(uint32_t events);
    // int echo();
    int handle_read();
    int handle_write();

    bool check_read() 
    {  
        return buf.get_blank_size() > 0 && could_read;
    }

    bool check_write()
    {
        return buf.get_exist_size() > 0 && could_write;
    }

    int recv_buf();
    int send_buf();

    int recv_buf_once();
    int send_buf_once();
};

#endif
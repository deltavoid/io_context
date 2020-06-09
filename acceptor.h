#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "io_context.h"


class Acceptor : public EpollHandler
{public:

    int fd;
    // struct sockaddr_in addr;
    IOContext* io_context;


    Acceptor(IOContext* io_context, struct sockaddr_in* addr, bool listen_ = false);
    virtual ~Acceptor();

    virtual int handle(uint32_t events);
    
    int listen(int backlog = 10);
    int accept();

};



#endif
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include <thread>

#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "io_context.h"
#include "acceptor.h"



int main(int argc, char** argv)
{
    if  (argc < 2)
    {   printf("usage: %s <port>\n", argv[0]);
        return 0;
    }

    int _port = 0;
    if  (sscanf(argv[1], "%d", &_port) < 0) perror("bad port");

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons((short)_port);
    bzero(&(addr.sin_zero), sizeof(addr.sin_zero));


    IOContext io_context;
    std::thread io_thread(
        [&io_context](){ io_context.run();} );

    Acceptor* acceptor = new Acceptor(&io_context, &addr, true);


    io_thread.join();

    return 0;
}
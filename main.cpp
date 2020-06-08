#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include <thread>

#include <unistd.h>

#include "io_context.h"






int main()
{
    IOContext io_context;

    std::thread io_thread( 
            [&io_context](){ io_context.run(); } );

    NotifyHandler notify_handler(&io_context);

    notify_handler.notify_queue.put(
            [](){ printf("hello world!\n");});
    uint64_t val = 1;
    if  (write(notify_handler.notify_fd, &val, sizeof(val)) < 0)
    {   perror("write eventfd error");
    }

    io_thread.join();


    return 0;
}
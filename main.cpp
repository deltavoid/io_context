#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include <thread>

#include <unistd.h>

#include "io_context.h"


void process(IOContext* io_context1, IOContext* io_context2);

void hello(IOContext* io_context, int i)
{
    printf("hello %c, thread: %lx\n", '0' + i, pthread_self());
    // just for demo, know what you are doing.
    sleep(1);
}

void deliver(IOContext* io_context1, IOContext* io_context2)
{
    printf("deliver: 1, thread: %lx\n", pthread_self());
    io_context2->notify_handler->put(
        [io_context1, io_context2](){ process(io_context2, io_context1);} );
    printf("deliver: 2\n");
}

void process(IOContext* io_context1, IOContext* io_context2)
{
    printf("------------------------------------------\n");
    printf("process: 1, thread: %lx\n", pthread_self());
    for (int i = 0; i < 5; i++)
        io_context1->put( 
            [io_context1, i](){ hello(io_context1, i);} );

    printf("process: 2\n");
    io_context1->put(
        [io_context1, io_context2](){ deliver(io_context1, io_context2);} );
    
    printf("process: 3\n");
}



int main()
{
    IOContext io_context1;
    // NotifyHandler notify_handler1(&io_context1);
    std::thread io_thread1( 
            [&io_context1](){ io_context1.run(); } );

    IOContext io_context2;
    // NotifyHandler notify_handler2(&io_context2);
    std::thread io_thread2( 
            [&io_context2](){ io_context2.run(); } );

    // notify_handler.put(
    //         [](){ printf("hello world, thread: %lx!\n", pthread_self());} );
    io_context1.notify_handler->put(
        [&io_context1, &io_context2](){ process(&io_context1, &io_context2);} );

    printf("main: thread_id: %lx\n", pthread_self());
    io_thread1.join();
    io_thread2.join();


    return 0;
}
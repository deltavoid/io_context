#ifndef BLOCKING_QUEUE_H
#define BLOCKING_QUEUE_H



// todo: 右值引用
template<typename Elem>
class BlockingQueue
{public:
    std::queue<Elem> queue;
    std::mutex mutex;
    std::condition_variable not_empty;

    BlockingQueue()
    {
    }

    ~BlockingQueue()
    {
    }

    void put(Elem x)
    {
        std::lock_guard<std::mutex> guard(mutex);
        queue.push(x);
        not_empty.notify_one();
    }

    Elem take()
    {
        std::unique_lock<std::mutex> lock(mutex);
        not_empty.wait(lock, [this] { return !queue.empty();});
        
        Elem x = queue.front();
        queue.pop();

        return x;
    }

    bool empty()
    {
        std::lock_guard<std::mutex> guard(mutex);
        return queue.empty();
    }

};



#endif
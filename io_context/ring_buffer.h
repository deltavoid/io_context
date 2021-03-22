#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <cstdint>
#include <sys/types.h>

class RingBuffer {
public:
    uint64_t f, p;
    char* buf;

    static const int max_pkt_size = 4096;

    RingBuffer()
    {
        f = p = 0;
        buf = new char[max_pkt_size];
    }

    ~RingBuffer()
    {
        delete buf;
    }

    int get_blank_size()
    {
        return max_pkt_size - 1 - (p - f);
    }

    int get_exist_size()
    {
        return  p - f;
    }

    int get_blank_iovec(struct iovec* iov, size_t* iov_len_p)
    {
        size_t& iov_len = *iov_len_p;
        int mf = f % max_pkt_size, mp = p % max_pkt_size;
        if  (mp < mf)
        {   iov[0].iov_base = buf + mp;
            iov[0].iov_len = mf - mp - 1;
            iov_len = 1;
        }
        else // mf <= mp
        {
            if  (mf == 0)
            {
                iov[0].iov_base = buf + mp;
                iov[0].iov_len = max_pkt_size - mp - 1;
                iov_len = 1;
            }
            else if  (mf == 1)
            {
                iov[0].iov_base = buf + mp;
                iov[0].iov_len = max_pkt_size - mp;
                iov_len = 1;
            }
            else
            {
                iov[0].iov_base = buf + mp;
                iov[0].iov_len = max_pkt_size - mp;
                iov[1].iov_base = buf;
                iov[1].iov_len = mf - 1;
                iov_len = 2;
            }
        }

        return 0;
    }

    int get_exist_iovec(struct iovec* iov, size_t* iov_len_p)
    {
        size_t& iov_len = *iov_len_p;
        int mf = f % max_pkt_size, mp = p % max_pkt_size;
        if  (mf <= mp)
        {   iov[0].iov_base = buf + mf;
            iov[0].iov_len = mp - mf;
            iov_len = 1;
        }
        else
        {   if  (mp == 0)
            {   iov[0].iov_base = buf + mf;
                iov[0].iov_len = max_pkt_size - mf;
                iov_len = 1;
            }
            else
            {   iov[0].iov_base = buf + mf;
                iov[0].iov_len = max_pkt_size - mf;
                iov[1].iov_base = buf;
                iov[1].iov_len = mp;
                iov_len = 2;
            }
        }

        return 0;
    }

    void add_blank(int v)
    {
        f += v;
    }

    void add_exist(int v)
    {
        p += v;
    }
};

#endif // RING_BUFFER_H
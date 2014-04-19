
#ifndef _recv_timeout_h__
#define _recv_timeout_h__

#include <poll.h>

const int READFLAG = POLLRDNORM;

inline bool recv_timeout(int fd,int secd)
{
    struct pollfd inputfd;
    inputfd.fd = fd;
    inputfd.events = READFLAG;
    int cnt = poll(&inputfd,1,secd*1000);
    if (cnt == 0)
    {
        //can not to read;timeout;
        return false;
    }
    if (cnt > 0 && (inputfd.revents & READFLAG))
    {
        return true;
    }
    return false;
}


#endif




#ifndef _socket_raw_h__
#define _socket_raw_h__

class CSocketRaw
{
public :
    //af => address family;
    CSocketRaw(int protocol,int af = -1);

    virtual ~CSocketRaw();

    //默认1s超时
    int recv_from_alarm(char *buf,int len,int timeout = 1);

    int recv_from_poll(char *buf,int len,int timeout = 1);

    int send_to(char *buf,int len);

    int fetch_fd() const 
    {
        return sockfd;
    }
private :
    int sockfd;
};

#endif //_socket_raw_h__


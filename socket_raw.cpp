
#include <netinet/ip_icmp.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>//recvfrom;
#include <unistd.h>//alarm
#include <stdio.h>
#include <exception>

#include "test_log.h"
#include "socket_raw.h"
#include "recv_timeout.h"


extern int errno;

extern int goalarm;

const int BUF_SIZE = 1024;

CSocketRaw::CSocketRaw(int protocol,int af)
{
    if (af == -1)
    {
        //据说，可以是PF_PACKET; 
        af = AF_INET;
    }
    sockfd = socket(af,SOCK_RAW,protocol);
    if (sockfd < 0)
    {
        char buf[BUF_SIZE] = {0};
        snprintf(buf,BUF_SIZE,"socket[protocol=%d] failed;errno[%d]:%s!\n",protocol,errno,strerror(errno));
        PRINT(buf);
        throw std::exception();
        return ;
    }
}

CSocketRaw::~CSocketRaw()
{
    close(sockfd);
}

int CSocketRaw::recv_from_poll(char *buf,int len,int timeout)
{
    if (!recv_timeout(sockfd,timeout))
    {
        return -1;
    }
    int sz = recvfrom(sockfd,buf,len,0,0,0);
    if (sz < 0)
    {
        char buf[BUF_SIZE] = {0};
        snprintf(buf,BUF_SIZE,"recvfrom error[%d] : %s!\n",errno,strerror(errno));
        printf(buf);
        return sz;
    }
    return sz;
}

int CSocketRaw::recv_from_alarm(char *buf,int len,int timeout)
{
    int sz = 0;
    alarm(timeout);
again:
    sz = recvfrom(sockfd,buf,len,0,0,0);
    if (sz < 0)
    {
        if (goalarm == 1)
        {
            return -1;
        }
        if (errno == EINTR)
        {
            //goto again;
            //这里就不注册中断函数了，所以中断直接返回失败.
        }
//#ifdef _MY_TEST_
        char buf[BUF_SIZE] = {0};
        snprintf(buf,BUF_SIZE,"recvfrom error[%d] : %s!\n",errno,strerror(errno));
        printf(buf);
//#endif//_MY_TEST_
        return sz;
    }
    return sz;
}

int CSocketRaw::send_to(char *buf,int len)
{
    int sz = 0;
again :
    sz = sendto(sockfd,buf,len,0,0,0);
    if (sz < 0)
    {
        if (errno == EINTR)
        {
            //goto again;
            //这里就不注册中断函数了，所以中断直接返回失败.
        }
        char buf[BUF_SIZE] = {0};
        snprintf(buf,BUF_SIZE,"recvfrom error[%d] : %s!\n",errno,strerror(errno));
        PRINT(buf);
        return sz;
    }
    return sz;
}












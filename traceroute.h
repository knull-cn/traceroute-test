
#ifndef _traceroute_h__
#define _traceroute_h__

#include <sys/time.h>
#include <netinet/in.h>
#include "socket_raw.h"

struct TRec
{
    short seq;
    short ttl;
    timeval tv;
};

struct TTraceInfo
{
    in_addr addr;
    time_t cost[3];
};

class CTraceroute
{
public :
    CTraceroute();

    ~CTraceroute();

    void traceroute_to(const char *dstip);

    void traceroute_to(int dstip);
private :
    int udp_init(int dstip);

    int udp_sendto(int ttl);

    int icmp_process(char *data,int len);

    int icmp_data_process(int type,int code,char *data,int len);

    void trace_info(int ttl,const TTraceInfo &info);

    time_t tv_sub();
private :
    //sendfd是UDP，在发送UDP数据的时候创建，结束的时候关闭;
    int sendfd_;
    //原始套接字，在初始化的时候就可以打开了;
    CSocketRaw sock;
    int rcvfd_;
    //目的地址,临时保存下.
    struct sockaddr_in daddr_;
    //源数据;
    struct sockaddr_in saddr_;
    //UDP发送的数据内容;
    TRec rec_;
    TTraceInfo info;
    //序列号，持续增加.
    long seq_;
    bool over_;
    //
    static int s_max_ttl_ ;
    static int s_start_port_;
    static int s_repeat_times_;
};

#endif //_traceroute_h__


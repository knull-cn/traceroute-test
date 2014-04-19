
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>//getpid;
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>//inet_pton;
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netdb.h>

#include "test_log.h"
#include "socket_raw.h"
#include "traceroute.h"

extern int errno;

int CTraceroute::s_start_port_ = 1500;
int CTraceroute::s_max_ttl_ = 30;
int CTraceroute::s_repeat_times_ = 3;

#define ERRORNO(INFO) DEBUG("%s failed!errno[%d]:%s!\n",INFO,errno,strerror(errno))

CTraceroute::CTraceroute():sock(IPPROTO_ICMP,AF_INET),seq_(0)
{
}

CTraceroute::~CTraceroute()
{
}

void CTraceroute::traceroute_to(const char *dstip)
{
    in_addr dst;
    if (inet_pton(AF_INET,dstip,&dst)<=0)
    {
        DEBUG("inet_pton failed!errno[%d]:%s!\n",errno,strerror(errno));
        return ;
    }
    traceroute_to(dst.s_addr);
}

void CTraceroute::traceroute_to(int dstip)
{
    const int BUF_SIZE = 1024;
    char buf[BUF_SIZE] ={0};
    int len = 0;
    if(0!=udp_init(dstip))
    {
        DEBUG("udp init failed!");
        return ;
    }
    int count = 0;
    over_ = false;
    for (int i=0;i!=s_max_ttl_;++i)
    {
        bzero(&info,sizeof(info));
        for (int j=0; j!=s_repeat_times_;++j)
        {
            //send udp;
            if(udp_sendto(i+1)<0)
            {
                DEBUG("send data failed!\n");
                break;
            }

            //recv icmp;先不管端口能够找到的情况.
            count = 0;
            while ((count++)<10)
            {
                len = sock.recv_from_poll(buf,BUF_SIZE);
                if (len > 0)
                {
                    //只有找到对应的UDP包的相应，那么才不再收取数据.
                    if (0 ==  icmp_process(buf,len))
                    {
                        info.cost[j] = tv_sub();
                        break;
                    }
                }
                else
                {
                    if (len < 0)
                    { 
                        info.cost[j] = -1;
                    }
                    else
                    {
                        info.cost[j] = 0;
                    }
                    break;
                }
            }
        }
        //
        trace_info(i+1,info);
        if (over_)
        {
            break;
        }
    }
    close(sendfd_);
}

void CTraceroute::trace_info(int ttl,const TTraceInfo &info)
{
    char buf[32] = {0};
    char name[128] = {0};
    if (info.addr.s_addr != 0)
    {
    if (NULL == inet_ntop(AF_INET,&info.addr,buf,32))
    {
        ERRORNO("trace_info :");
        return ;
    }
    struct sockaddr_in dstaddr;
    dstaddr.sin_family = AF_INET;
    dstaddr.sin_addr = info.addr;
    dstaddr.sin_port = 0;
    if (getnameinfo((struct sockaddr *)&dstaddr,sizeof(dstaddr),name,128,NULL,0,0) < 0)
    {
        ERRORNO("trace_info:");
        return ;
    }
    printf("%d %s (%s):",ttl,name,buf);
    }
    else
    {
        printf("%d ",ttl);
    }
    for (int i=0;i!=3;++i)
    {
        if (info.cost[i] == -1)
        {
            printf(" * ");
        }
        else
        {
            printf(" %u ms ",info.cost[i]);
        }
    }
    printf("\n");
}

int CTraceroute::udp_init(int dstip)
{
    sendfd_ = socket(AF_INET,SOCK_DGRAM,0);
    if (sendfd_ < 0)
    {
        ERRORNO("udp_init socket");
        return -1;
    }
    saddr_.sin_family = AF_INET;
    saddr_.sin_addr.s_addr = INADDR_ANY;
    short port = getpid()&0xffff;
    saddr_.sin_port = htons(port | 0x8000);
    if (bind(sendfd_,(struct sockaddr*)&saddr_,sizeof(saddr_)))
    {
        ERRORNO("udp_init bind");
        close(sendfd_);
        return -2;
    }
    daddr_.sin_family = AF_INET;
    daddr_.sin_port = htons(s_start_port_);
    daddr_.sin_addr.s_addr = dstip;
    return 0;
}

int CTraceroute::udp_sendto(int ttl)
{
    //set ttl;
    int ret = setsockopt(sendfd_,IPPROTO_IP,IP_TTL,&ttl,sizeof(int));
    if (ret < 0)
    {
        ERRORNO("udp_sendto setsockopt");
        return -1;
    }
    //construct udp data;
    rec_.seq = ++seq_;
    rec_.ttl = ttl;
    gettimeofday(&(rec_.tv),0);
    ret = sendto(sendfd_,(void*)&rec_,sizeof(rec_),0,(struct sockaddr*)&daddr_,sizeof(daddr_));
    if (ret < 0)
    {
        ERRORNO("udp_sendto sendto");
        return -2;
    }
}

int CTraceroute::icmp_process(char* data,int len)
{
    //1-iphdr;
    struct iphdr *pip = (struct iphdr*)data;
    if (pip->protocol != IPPROTO_ICMP)
    {
        DEBUG("protocol [%d] is not ICMP!\n");
        return -1;
    }
    int iphdrlen = pip->ihl<<2;//4字节为单位的.
    if (len < iphdrlen + 8)
    {
        DEBUG("ip data len error [%d],hdr len [%d]!\n",len,iphdrlen);
        return -2;
    }
    //2-icmphdr;
    struct icmphdr *picmp = (struct icmphdr*)(data+iphdrlen);
    int ret =  icmp_data_process(picmp->type,picmp->code,data+iphdrlen+8,len-iphdrlen-8);
    if (info.addr.s_addr == 0)
    {
        if (ret == 0)
        {
            info.addr.s_addr = pip->saddr;
        }
    }
    return ret;
}

int CTraceroute::icmp_data_process(int type,int code,char *data,int len)
{
    //check type;
    if (type != ICMP_TIME_EXCEEDED && type != ICMP_DEST_UNREACH)
    {
        DEBUG("Un accept type [%d]!code is [%d]\n",type,code);
        return -10;
    }
    //check iphdr of icmp packet;
    if (len < sizeof(iphdr))
    {
        DEBUG("icmp data is too less;len [%d]\n",len);
        return -11;
    }
    struct iphdr *pip = (struct iphdr *)data;
    if (pip->protocol != IPPROTO_UDP)
    {
        DEBUG("traceroute recv icmp only for udp!But this data protocol is [%d]\n",pip->protocol);
        return -12;
    }
    if (pip->ihl<<2 + 4 < len)
    {
        DEBUG("Data too less!");
        return -13;
    }
    //check udp;
    struct udphdr *pudp = (struct udphdr*)(data+(pip->ihl<<2));
    if (pudp->source != saddr_.sin_port || pudp->dest != daddr_.sin_port)
    {
        DEBUG("Not from this traceroute!\n");
        return -14;
    }
    //data process!
    //---
    int ret = 0;
    switch(type)
    {
        case ICMP_TIME_EXCEEDED:
            {
                if (code == ICMP_EXC_TTL)
                {
                    DEBUG("ICMP_TIME_EXCEEDED code is [%d]\n",code);
                }
                else
                {
                    DEBUG("ICMP_TIME_EXCEEDED code is [%d]\n",code);
                }
                break;
            }
        case ICMP_DEST_UNREACH:
            {
                if (code == ICMP_PORT_UNREACH)
                {
                    over_ = true;//reach the destnation;
                }
                DEBUG("ICMP_DEST_UNREACH code is [%d]\n",code);
                break;
            }
        default :
            {
                DEBUG("type[%d] code is [%d]\n",type,code);
                ret = -15;
                break;
            }
    }
    return 0;
}

time_t CTraceroute::tv_sub()
{
    timeval tval;
    gettimeofday(&tval,0);
    DEBUG("\t time in send [%u:%u]\n",rec_.tv.tv_sec,rec_.tv.tv_usec);
    DEBUG("\t time in recv [%u:%u]\n",tval.tv_sec,tval.tv_usec);
    long rlt = (tval.tv_sec-rec_.tv.tv_sec)*1000 + (tval.tv_usec - rec_.tv.tv_usec)/1000;
    if (rlt < 0)
    {
        rlt = 0;
    }
    return rlt;
}












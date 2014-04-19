
#include <netinet/ip.h>
#include "socket_icmp.h"

#include <stdio.h>

//用于调试信息输出.
#ifdef _MY_TEST_
#define DEBUG printf 
#else
#define DEBUG /*empty*/
#endif//_MY_TEST_

//用于实际信息输出;
#define PRINT printf 

CSocketIcmp::CSocketIcmp():CSocketRaw(IPPROTO_ICMP,AF_INET),fp_icmp_print(NULL)
{
}

int CSocketIcmp::process(const char *ippayload,int len)
{
    const struct iphdr *pip = (const struct iphdr*)(ippayload);
    //check protocol;
    if (pip->protocol != IPPROTO_ICMP)
    {
        //log;
        return -1;
    }
    //ip头长度，是以4字节为单位的.
    int iphdrlen = pip->ihl<<2;
    //at least 8 char;
    if (len - iphdrlen < 8)
    {
        return -2;
    }
    //process icmp;
    const struct icmphdr *picmp = (const struct icmphdr *)(ipplayload + iphdrlen);
    const char *icmpdata = ipplayload + iphdrlen + sizeof(icmphdr);
    if (fp_icmp_print == NULL)
    {
        default_icmp_print(picmp->type,picmp->code,icmpdata,icmpdatalen);
    }
    else
    {
        fp_icmp_print(picmp->type,picmp->code,icmpdata,icmpdatalen);
    }
    return 0;
    switch(picmp->type)
    {
        case ICMP_ECHOREPLY:
            {
                fp_icmp_print(picmp->code,icmpdata,icmpdatalen);
                break;
            }
        case ICMP_DEST_UNREACH:
            {
                fp_icmp_print(picmp->code,icmpdata,icmpdatalen);
                break;
            }
        case ICMP_TIME_EXCEEDED:
            {
                fp_icmp_print(picmp->code,icmpdata,icmpdatalen);
                break;
            }
        default :
            {
                //log;
                DEBUG("icmp type[%d] and code[%d]!\n",picmp->type,picmp->code);
                break;
            }
    }
    return 0;
}

int CSocketIcmp::default_icmp_print(int type,int icmp_code,void *data,int len)
{
    DEBUG("icmp type[%d] and code [%d];\n\ticmp data len[%d]\n",type,icmp_code,len);
    return 0;
}

int CSocketIcmp::icmp_echoreply(int code,const char *icmpdata,int len)
{
    return 0;
}

int CSocketIcmp::icmp_dest_unreach(int code,const char *icmpdata,int len)
{
    return 0;
}

//如果是time_exceeded，那么icmpdata中包含iphdr+icmphdr数据（源ICMP数据包）.
int CSocketIcmp::icmp_time_exceeded(int code,const char *icmpdata,int len)
{
    if (len <=0)
    {
        DEBUG("icmp_time_exceeded :data len %d!\n",len);
        return -1;
    }
    int ret = 0;
    switch(code)
    {
        case ICMP_EXC_TTL:
            {
                DEBUG("time exceeded code [%d]:ICMP_EXC_TTL!\n",code);
                if(0 != time_exceeded(icmp_data,len))
                {
                    ret = -3;
                }
                break;
            }
        case ICMP_EXC_FRAGTIME:
            {
                DEBUG("time exceeded code [%d]:ICMP_EXC_FRAGTIME!\n",code);
                if (0 !=  time_exceeded(icmp_data,len))
                {
                    ret = -4;
                }
                break;
            }
        default :
            {
                DEBUG("Unknow icmp code [%d]\n",code);
                ret = -5;
                break;
            }
    }
    
    return ret;
}

int CSocketIcmp::time_exceeded(const char *icmpdata,int len)
{
    //处理源数据的IP头和包含的下层协议的12个字节（可能是UDP／TCP／ICMP等协议头).
    const struct iphdr *pip = (const struct  iphdr*)icmpdata;
    int iphdrlen = pip->ihl<<2;
    const char *pipdata = icmpdata + iphdrlen;
    int ret = 0;
    switch(pip->protocol)
    {
        case IPPROTO_UDP:
            {
                const struct udphdr* pudp = (const struct udphdr*)(pipdata);
                if (icmp_data_check())
                {
                    gettimeofday(dd,0);
                }
                break;
            }
        default :
            {
                DEBUG("Unknow ip->protocol[%d] of icmp response!\n",pip->protocol);
                ret = -1;
                break;
            }
    }
    return ret;
}











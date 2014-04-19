#include <netinet/ip.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
//#include <stdlib.h>
#include <unistd.h>

#include "traceroute.h"
#include "socket_raw.h"

extern int errno;

int goalarm = 0;

void sig_alarm(int signo)
{
    printf("==========alarm clock!==============\n");
    goalarm = 1;
}

int test_sock_raw()
{
    CSocketRaw sockobj(IPPROTO_ICMP);
    char buf[2048] = {0};
    while (sockobj.recv_from_poll(buf,2048)>0)
    {
        struct iphdr* pip = (struct iphdr*)buf;
        if (pip->protocol == IPPROTO_TCP)
        {
            printf("get one IPPROTO_TCP\n");
            continue;
        }
        if (pip->protocol == IPPROTO_UDP)
        {
            printf("get one IPPROTO_UDP[%d]\n",IPPROTO_UDP);
            continue;
        }
        continue;
    }
    return 0;
}

void test_traceroute(const char *name)
{
    CTraceroute traceroute;
    struct hostent *phost = gethostbyname(name);
    if (phost == NULL)
    {
        printf("gethostbyname(%s) failed!errno[%d]:%s!\n",name,errno,strerror(errno));
        return ;
    }
    char straddr[32] = {0};
    if (phost->h_addr_list[0] == NULL)
    {
        printf("[%s] is no addr!\n");
        return ;
    }
    inet_ntop(AF_INET,phost->h_addr_list[0],straddr,32);

    printf("traceroute to %s (%s)\n",name,straddr);
    traceroute.traceroute_to(straddr);
}

void test_alarm()
{
    alarm(3);
    char buf[1024] = {0};
    int len = read(STDIN_FILENO,buf,1024);
    alarm(0);
    printf("test alarm succed!\n");
}

int main(int argc,char *argv[])
{
    signal(SIGALRM,sig_alarm);
    const char *pname = "www.baidu.com";
    if (argc == 2)
    {
        pname = argv[1];
    }
    test_traceroute(pname);
    return 0;

}








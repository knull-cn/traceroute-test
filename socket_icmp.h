
#ifndef _socket_icmp_h__
#define _socket_icmp_h__

#include <netinet/ip_icmp.h>
#include <sys/time.h>//for timeval;
#include "socket_raw.h"

typedef int (*FPIcmpPrint)(int icmp_type,int icmp_code,void *data,int len);

class CSocketIcmp : public CSocketRaw
{
public :

private :
    FPIcmpPrint fp_icmp_print;
};


#endif //_socket_icmp_h__



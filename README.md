traceroute-test
===============

programming exercise(原始套接字编程练习)

test_log.h 是一个简单的日志函数，即是printf函数。好处是，在调试完成后可以直接关闭该日志。

socket_icmp.h/cpp是个失败的代码；因为ICMP有特定的功能性，某个程序只需要其中的一种功能(比如ping只需要回射，traceroute只需要ICMP_TIME_EXCEEDED+ICMP_DEST_UNREACH），而ICMP消息种类太多，我没有能力，也没有必要去全面的处理。所以，专门写个ICMP类，是没有必要的.

socket_raw.h/cpp是用来处理原始套接字的。功能比较简单，仅仅是读写.由于我的系统alarm信号处理似乎有问题，所以从新用poll来实现超时recv（具体代码在recv_timeout.h)

traceroute.h/cpp是实现traceroute的主要功能代码.

main.cpp写的一些测试函数.

暂时存在的问题：由于第一次写，许多东西都不知道、不懂，所以仅仅是功能完成；调理不是特别清楚.

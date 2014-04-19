#include <signal.h>
#include <unistd.h>
#include <stdio.h>

void sig_alarm(int )
{
    printf("==========alarm clock!==============\n");
}
void test_alarm()
{
    alarm(3);
    char buf[1024] = {0};
    int len = read(STDIN_FILENO,buf,1024);
    alarm(0);
    printf("test alarm succed!\n");
}

int main()
{
    signal(SIGALRM,sig_alarm);
    test_alarm();
    return 0;
}

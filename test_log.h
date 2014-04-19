
#ifndef _test_log_h__
#define _test_log_h__

//#ifndef DEBUG
//用于调试信息输出.
#ifdef _MY_TEST_
#include <stdio.h>
#define DEBUG printf 
#define PRINT printf
#else
#define DEBUG /*empty*/
#endif//_MY_TEST_

//#endif //ifndef DEBUG

#endif  //_test_log_h__


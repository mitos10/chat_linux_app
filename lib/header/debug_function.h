#ifndef DEBUG_FUNCTION_H
#define DEBUG_FUNCTION_H

#include "debug_opt.h"

#if __RECORD_ALL__

#endif

#define __DEBUG_NULL__\
    fprintf(stdout, "__DEBUG_NULL__ FILE: %s LINE %d FUNCTION: %s\n", __FILE__, __LINE__, __FUNCTION__);

#define __DEBUG_TEST1__\
    fprintf(stdout, "__DEBUG_TEST1__ FILE: %s LINE %d\n", __FILE__, __LINE__);

#define __DEBUG_SOCK_WRITE_Q__\
    printf("Send Queue\n");             \
	listPrint(&_UsrSck_h.sndQ, stdout); \
	printf("Rcv Queue\n");              \
	listPrint(&_UsrSck_h.rcvQ, stdout); \
	fflush(stdout);                     \
    fprintf(stdout, "__DEBUG_TEST1__ FILE: %s LINE %d\n", __FILE__, __LINE__);

#define __DEBUG_TEST2__\
    fprintf(stdout, "__DEBUG_TEST2__ FILE: %s LINE %d\n", __FILE__, __LINE__);

#define __ASSERT_NULL__(CONDITION)\
    fprintf(stdout, "__ASSERT_NULL__ FAILED ON CONDITION %s FILE: %s LINE %d FUNCTION: %s\n", #CONDITION, __FILE__, __LINE__, __FUNCTION__);

#define __ASSERT_TEST1__(CONDITION)\
    fprintf(stdout, "__ASSERT_TEST1__ FAILED ON CONDITION %s FILE: %s LINE %d\n", #CONDITION, __FILE__, __LINE__);

#endif
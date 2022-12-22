#ifndef DEBUG_FUNCTION_H
#define DEBUG_FUNCTION_H

#include "debug_opt.h"

#if __RECORD_ALL__

#endif

#define __DEBUG_NULL__\
    fprintf(stdout, "__DEBUG_NULL__ FILE: %s LINE %d FUNCTION: %s\n", __FILE__, __LINE__, __FUNCTION__);

#define __DEBUG_BACKTRACE__\
  fprintf(stdout, "-------------------------------------\
--------------------------------------------------------\
---------------------------------------------------\n");\
  void *trace[16];                                      \
  char **messages = (char **)NULL;                      \
  int i, trace_size = 0;                                \
  /*Translation of traces to symbols*/                  \
  trace_size = backtrace(trace, 16);                    \
  messages = backtrace_symbols(trace, trace_size);      \
  fprintf(stdout, "CALLSTACK:\n");                      \
  /*Print each call stack element*/                     \
  for (i=0; i<trace_size; ++i)                          \
  {                                                     \
    /*printf("[bt] #%d %s\n", i, messages[i]);*/        \
    size_t p = 0, q = 0;                                \
    /*Extract object name*/                             \
    while(messages[i][p] != '(' && messages[i][p] != ' '\
            && messages[i][p] != 0)                     \
        ++p;                                            \
    q = p + 2;                                          \
    /*Extract address*/                                 \
    while(messages[i][q] != ')' && messages[i][p] != ' '\
            && messages[i][p] != 0)                     \
        ++q;                                            \
    /*Make syscall to addr2line*/                       \
    char syscom[256];                                   \
    sprintf(syscom,"addr2line %.*s -e %.*s",            \
    (int)q- (int)p - 2, &messages[i][p+2],              \
    (int)p, messages[i]);                               \
    /*printf("%s\n", syscom);*/                         \
    system(syscom);                                     \
  }                                                     \
  fprintf(stdout, "-------------------------------------\
--------------------------------------------------------\
---------------------------------------------------\n");\
  fflush(stdout);

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

#define __ASSERT_BACKTRACE__(CONDITION)\
  fprintf(stdout, "__ASSERT_BACKTRACE__ FAILED ON CONDITION\
 %s FILE: %s LINE %d FUNCTION: %s\n", #CONDITION,          \
__FILE__, __LINE__, __FUNCTION__);                         \
  __DEBUG_BACKTRACE__

#define __ASSERT_TEST1__(CONDITION)\
    fprintf(stdout, "__ASSERT_TEST1__ FAILED ON CONDITION %s FILE: %s LINE %d\n", #CONDITION, __FILE__, __LINE__);

#endif
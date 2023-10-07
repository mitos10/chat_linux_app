#include "debug_test_macros.h"
#include <stdio.h>

void stack_test2(){
    printf("Inside stack test2");
    __DEBUG_PRINT__(__DEBUG_BACKTRACE__)
    printf("Outside stack test2");
}

void stack_test1(){
    printf("Inside stack test1");
    stack_test2();
    printf("Outside stack test1");
}

void main(){
    __DEBUG_PRINT__(__DEBUG_NULL__);
    int i = 1;
    __ASSERT__(i>10,__ASSERT_NULL__)
    i = 0;
    __ASSERT__(i<-1,__ASSERT_TEST1__)
    stack_test1();
    return;
}

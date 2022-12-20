#include "debug_test_macros.h"
#include <stdio.h>

void main(){
    __DEBUG_PRINT__(__DEBUG_NULL__);
    int i = 1;
    __ASSERT__(i>10,__ASSERT_NULL__)
    i = 0;
    __ASSERT__(i<-1,__ASSERT_TEST1__)

    return;
}
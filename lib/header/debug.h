#ifndef DEBUG_H
#define DEBUG_H

#include "debug_opt.h"
#include "debug_function.h"

    #if __DEBUG__
        #define __DEBUG_PRINT__(PRINT_CALLBACK_FUNCTION) \
            PRINT_CALLBACK_FUNCTION
    #else
        #define __DEBUG_PRINT__
    #endif

    #if __SANITY_CHECKS__
        #define __ASSERT__(CONDITION, ASSERT_CALLBACK_FUNCTION) \
            do{                                                 \
                if(!(CONDITION)){                               \
                    ASSERT_CALLBACK_FUNCTION(CONDITION)         \
                }                                               \
            }while(FALSE);
    #else 
        #define __ASSERT__
    #endif

    #if __MEMORY__
        #include "memory.h"
    #endif

#endif
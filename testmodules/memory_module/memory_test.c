#include "memory_test.h"


 main(){
     char id_test[50] = "Memory test 1";
     init_memory(id_test, 20000);
     print_mem("Memory test 1");
     char* ptr1= cmalloc(215, "Memory test 1");
     print_mem("Memory test 1");
     char* ptr2= cmalloc(215,"Memory test 1");
     print_mem("Memory test 1");
     char* ptr3 = cmalloc(215,"Memory test 1");
     printf("%p %p %p\n",ptr1, ptr2, ptr3);
     print_mem("Memory test 1");
     cfree(ptr2, "Memory test 1");
     print_mem("Memory test 1");
     cmalloc(512,"Memory test 1");
     print_mem("Memory test 1");
     cfree(ptr1,"Memory test 1");
     print_mem("Memory test 1");
     cmalloc(215,"Memory test 1");
     print_mem("Memory test 1");
     cmalloc(215,"Memory test 1");
     print_mem("Memory test 1");
     cfree(ptr3,"Memory test 1");
     print_mem("Memory test 1");
 }
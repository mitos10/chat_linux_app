#include "memory_test.h"


int main(){
     char id_test[50] = "Memory test 1";
     init_memory(id_test, 20000);
     //print_mem("Memory test 1");
     char* ptr1= monitor_cmalloc(215, "Memory test 1", __FILE__, __LINE__);
     monitor_print("Memory test 1");
     //print_mem("Memory test 1");
     char* ptr2= monitor_cmalloc(215,"Memory test 1", __FILE__, __LINE__);
     monitor_print("Memory test 1");
     //print_mem("Memory test 1");
     char* ptr3 = monitor_cmalloc(215,"Memory test 1", __FILE__, __LINE__);
     monitor_print("Memory test 1");
     printf("%p %p %p\n",ptr1, ptr2, ptr3);
     //print_mem("Memory test 1");
     monitor_cfree(ptr2, "Memory test 1");
     monitor_print("Memory test 1");
     //print_mem("Memory test 1");
     monitor_cmalloc(512,"Memory test 1", __FILE__, __LINE__);
     monitor_print("Memory test 1");
     //print_mem("Memory test 1");
     monitor_cfree(ptr1,"Memory test 1");
     monitor_print("Memory test 1");
     //print_mem("Memory test 1");
     monitor_cmalloc(215,"Memory test 1", __FILE__, __LINE__);
     monitor_print("Memory test 1");
     //print_mem("Memory test 1");
     monitor_cmalloc(215,"Memory test 1", __FILE__, __LINE__);
     monitor_print("Memory test 1");
     //print_mem("Memory test 1");
     monitor_cfree(ptr3,"Memory test 1");
     monitor_print("Memory test 1");
     //print_mem("Memory test 1");
 }
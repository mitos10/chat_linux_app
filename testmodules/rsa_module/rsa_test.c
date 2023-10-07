#include "../../lib/header/rsa.h"

void main(){
    Memory_DefaultInit(10000);
    long_num num1 = _generateLongNum(20);
    long_num num2 = _generateLongNum(10);

    _sumLongNum(num1, num2, &num1);
}
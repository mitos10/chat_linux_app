#include "../header/rsa.h"

long_num _generateLongNum(uint16_t size){
    long_num ret_num = {
        .num = (uint8_t *) malloc( sizeof(uint8_t)* size ),
        .size = size,
    };
    
    if(ret_num.num){
        srand(time(NULL));
        for(uint16_t i = 0; i < ret_num.size; i++)
            ret_num.num[i] = rand() % 256;
    }
    return ret_num;
}

// unsigned long long gcd(unsigned long long a, unsigned long long b){
//     unsigned long long aux = 0;
//     while(b != 0){
//         aux = b;
//         b = a%b;
//         a = b;
//     }
//     return aux;
// }

// unsigned long long pow_mod(unsigned long long base, unsigned long long exp, unsigned long long mod){
//     unsigned long long result = 1;
//     while(exp--){
//         result *= base;
//         result %= mod;
//     }
//     return result;

//     while(exp--){
//         result = (exp & 0x1) * (result * base) % mod;
//         base = (base * base) % mod;
//         exp >>=1;
//     }
//     return result;
// }

long_num _multLongNum(long_num op1, long_num op2){
    long_num result_num = {
        .num = malloc(sizeof(char) * (op1.size + op2.size) ),
        .size = op1.size + op2.size,
    };
    memset(result_num.num, 0, result_num.size);
    for(uint16_t i = 0; i <op1.size; i++)
        _sumLongNum();

    return result_num;
}

typedef enum {
    SHIFT_RIGHT = 0,
    SHIFT_LEFT = 1,
}SHIFT_DIR;

long_num _shiftLongNum(long_num op1, uint16_t shift, SHIFT_DIR dir){
    long_num res = {
        .num = malloc(dir == SHIFT_LEFT ? op1.size + shift : op1.size),
        .size = dir == SHIFT_LEFT ? op1.size + shift : op1.size,
    };
    memset(res.num,0,res.size);
    
}

void _sumLongNum(long_num op1, long_num op2, long_num* result){
    uint16_t limit = op1.size > op2.size? op1.size : op2.size;

    uint8_t op1_changed = 0;
    uint8_t op2_changed = 0;

    _printLongNum(op1);
    _printLongNum(op2);

    //change op1 size to larger operand size
    if(op1.size < limit){
        //allocate and copy new size
        uint8_t* aux_num =(uint8_t*) malloc(limit);
        memset(aux_num, 0 , limit);
        //copy number to end of array
        memcpy(aux_num, op1.num, op1.size);
        op1.num = aux_num;
        op1.size = limit;
        op1_changed = 1;
    }

    //change op2 size to larger operand size
    if(op2.size < limit){
        //allocate and copy new size
        uint8_t* aux_num =(uint8_t*) malloc(limit);
        memset(aux_num, 0 , limit);
        //copy number to end of array
        memcpy(aux_num, op2.num, op2.size);
        op2.num = aux_num;
        op2.size = limit;
        op2_changed = 1;
    }

    _printLongNum(op1);
    _printLongNum(op2);

    //change result number size to larger operand size
    if(result->size < limit){
        //allocate and copy new size
        uint8_t* aux_num =(uint8_t*) malloc(limit);
        memset(aux_num, 0 , limit);
        //copy number to end of array
        memcpy(aux_num, result->num, result->size);
        free(result->num);
        result->num = aux_num;
        result->size = limit;
    }

    uint8_t carry = 0;
    for(uint16_t i = 0; i < limit; i++){
        uint16_t sum = (uint16_t) op1.num[i] + (uint16_t)op2.num[i] + (uint16_t) carry;
        carry = sum >> 8;
        result->num[i] = (uint8_t) (sum & 0xFF);
    }

    _printLongNum(*result);

    if(op1_changed)
        free(op1.num);
    if(op2_changed)
        free(op2.num);

}

long_num _modLongNum(long_num num1, long_num num2){

}

void _printLongNum(long_num num){
    for(uint16_t i = 0; i < num.size; i++)
        printf("%.2X ", num.num[i]);
    printf("\n");
}
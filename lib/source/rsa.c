unsigned long long gcd(unsigned long long a, unsigned long long b){
    unsigned long long aux = 0;
    while(b != 0){
        aux = b;
        b = a%b;
        a = b;
    }
    return aux;
}

unsigned long long pow_mod(unsigned long long base, unsigned long long exp, unsigned long long mod){
    unsigned long long result = 1;
    while(exp--){
        result *= base;
        result %= mod;
    }
    return result;

    while(exp--){
        result = (exp & 0x1) * (result * base) % mod;
        base = (base * base) % mod;
        exp >>=1;
    }
    return result;
}

char* multiply_large(char* op1, int sz_op1, char* op2, int sz_op2){
    char* result = calloc(sizeof(char) * sz_op1 * sz_op2);
    
}
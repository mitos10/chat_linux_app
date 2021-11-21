#include "sha256.h"

h_state const h_init = { 
    .a = 0x6a09e667,
    .b = 0xbb67ae85,
    .c = 0x3c6ef372,
    .d = 0xa54ff53a,
    .e = 0x510e527f,
    .f = 0x9b05688c,
    .g = 0x1f83d9ab,
    .h = 0x5be0cd19
};

uint32_t const k[] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

int pad_pass(char* pass, char** padded_pass);
void init_w(uint32_t w[64], char* padded_pass, int i);

uint32_t Ch(uint32_t e, uint32_t f, uint32_t g);
uint32_t s0(uint32_t a);
uint32_t s1(uint32_t e);
uint32_t ma(uint32_t a, uint32_t b, uint32_t c);

uint32_t r_rot(uint32_t reg,uint8_t rot);
void sum_h_prev_h(h_state *sha256_hash, h_state *sha256_prev);

char* endianness_sha(h_state *sha256_hash);

char* sha256sum( char *pass){
    
    h_state* sha256_hash = (h_state*) calloc(1,sizeof(h_state));
    h_state* sha256_prev = (h_state*) calloc(1,sizeof(h_state));
    *sha256_hash = h_init;

    char* padded_pass = NULL;
    int size = pad_pass(pass, &padded_pass);

    for(int i = 0; i < size / SHA_CHUNK_SIZE; i++ ){
        
        *sha256_prev = *sha256_hash;

        uint32_t w[64] = {0};
        
        init_w(w,padded_pass,i);
        
        for(int ii = 0; ii < 64; ii++){

            uint32_t sum_wk_v = w[ii] + k[ii];
            uint32_t Ch_v = Ch(sha256_hash->e,sha256_hash->f,sha256_hash->g);
            uint32_t ma_v = ma(sha256_hash->a,sha256_hash->b,sha256_hash->c);
            uint32_t s0_v = s0(sha256_hash->a);
            uint32_t s1_v = s1(sha256_hash->e);
            
            uint32_t temp1 = sha256_hash->h + sum_wk_v + Ch_v + s1_v;
            uint32_t temp2 = s0_v + ma_v;

            sha256_hash->h = sha256_hash->g;
            sha256_hash->g = sha256_hash->f;
            sha256_hash->f = sha256_hash->e;
            sha256_hash->e = sha256_hash->d + temp1;
            sha256_hash->d = sha256_hash->c;
            sha256_hash->c = sha256_hash->b;
            sha256_hash->b = sha256_hash->a;
            sha256_hash->a = temp1 + temp2;
        }
            
        sum_h_prev_h(sha256_hash, sha256_prev);
    }

    char* sha_256_str = endianness_sha(sha256_hash);
    
    free(padded_pass);
    free(sha256_hash);
    
    return sha_256_str;
}

int pad_pass(char* pass, char** padded_pass){
    uint64_t len = strlen(pass)*8;
    uint64_t footer_len = htobe64(len);
    int total_size = ceil( (float) (len / 8 + 8) / SHA_CHUNK_SIZE) * SHA_CHUNK_SIZE;
    *padded_pass = (char*) calloc( total_size, sizeof(char));
    strcpy(*padded_pass, pass);
    (*padded_pass)[len/8] = 0x80;
    memcpy(& ((*padded_pass)[total_size - 8]), &footer_len, 8);
    return total_size;
}

void init_w(uint32_t w[64], char* padded_pass, int i){
    
    uint32_t* aux_pass = (uint32_t*) padded_pass;

    for(int i = 0; i < 16; i++)
        w[i] = ntohl(aux_pass[i]);

    for(int i = 16; i < 64; i++){
        uint32_t s0 = r_rot(w[i -15],7) ^ r_rot(w[i - 15], 18) ^ (w[i - 15] >> 3);
        uint32_t s1 = r_rot(w[i -2],17) ^ r_rot(w[i - 2], 19) ^ (w[i - 2] >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }

}

uint32_t Ch(uint32_t e, uint32_t f, uint32_t g){
   return ( e & f ) ^ ( (~e) & g );
}

uint32_t s0(uint32_t a){
    return r_rot(a, 2) ^ r_rot(a, 13) ^ r_rot(a, 22);
}

uint32_t s1(uint32_t e){
    return r_rot(e, 6) ^ r_rot(e, 11) ^ r_rot(e, 25);
}

uint32_t ma(uint32_t a, uint32_t b, uint32_t c){
    return (a & b) ^ (a & c) ^ (b & c); 
}

uint32_t r_rot(uint32_t reg,uint8_t rot){
    return (reg << (sizeof(uint32_t) * 8 - rot)) | (reg >> rot);  
}

void sum_h_prev_h(h_state *sha256_hash, h_state *sha256_prev){
    
    uint32_t *aux_hash = (uint32_t*) sha256_hash;
    uint32_t *aux_prev = (uint32_t*) sha256_prev;

    for(int i = 0; i < 8; i++)
        aux_hash[i] += aux_prev[i];

}

char* endianness_sha(h_state *sha256_hash){
    uint32_t *sha_out =  (uint32_t*) calloc(8, sizeof(uint32_t));
    uint32_t *aux = (uint32_t*) sha256_hash;
    for(int i = 0; i < 8; i++)
        sha_out[i] = htonl(aux[i]);
    return (char*) sha_out;
}
#include "../header/sha256.h"

#define R_ROT(reg, rot) ((reg << (sizeof(uint32_t) * 8 - rot)) | (reg >> rot))
#define CH(e, f, g) (( e & f ) ^ ( (~e) & g ))
#define S0(a)  (R_ROT(a, 2) ^ R_ROT(a, 13) ^ R_ROT(a, 22))
#define S1(e) (R_ROT(e, 6) ^ R_ROT(e, 11) ^ R_ROT(e, 25))
#define MA(a,b,c) ((a & b) ^ (a & c) ^ (b & c))

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

/**
 * @brief 
 * 
 * @param buffer 
 * @param padded_buffer 
 * @return int 
 */
static int pad_buffer(char* buffer, char** padded_buffer);
/**
 * @brief 
 * 
 * @param w 
 * @param padded_buffer 
 * @param i 
 */
static void init_w(uint32_t w[64], char* padded_buffer, int i);
/**
 * @brief 
 * 
 * @param sha256_hash 
 * @param sha256_prev 
 */
static void sum_h_prev_h(h_state *sha256_hash, h_state *sha256_prev);
/**
 * @brief 
 * 
 * @param sha256_hash 
 * @return char* 
 */
static char* endianness_sha(h_state *sha256_hash);

char* SHA256_CheckSum(char *buffer){
    
    h_state* sha256_hash = (h_state*) calloc(1,sizeof(h_state));
    h_state* sha256_prev = (h_state*) calloc(1,sizeof(h_state));
    *sha256_hash = h_init;

    char* padded_buffer = NULL;
    int size = pad_buffer(buffer, &padded_buffer);

    for(int i = 0; i < size / SHA_CHUNK_SIZE; i++ ){
        
        *sha256_prev = *sha256_hash;

        uint32_t w[64] = {0};
        
        init_w(w,padded_buffer,i);
        
        for(int ii = 0; ii < 64; ii++){

            uint32_t sum_wk_v = w[ii] + k[ii];
            uint32_t Ch_v = CH(sha256_hash->e,sha256_hash->f,sha256_hash->g);
            uint32_t ma_v = MA(sha256_hash->a,sha256_hash->b,sha256_hash->c);
            uint32_t s0_v = S0(sha256_hash->a);
            uint32_t s1_v = S1(sha256_hash->e);
            
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
    
    free(padded_buffer);
    free(sha256_hash);
    
    return sha_256_str;
}

static int pad_buffer(char* buffer, char** padded_buffer){
    uint64_t len = strlen(buffer)*8;
    uint64_t footer_len = htobe64(len);
    int total_size = ceil( (float) (len / 8 + 8) / SHA_CHUNK_SIZE) * SHA_CHUNK_SIZE;
    *padded_buffer = (char*) calloc( total_size, sizeof(char));
    strcpy(*padded_buffer, buffer);
    (*padded_buffer)[len/8] = 0x80;
    memcpy(& ((*padded_buffer)[total_size - 8]), &footer_len, 8);
    return total_size;
}

static void init_w(uint32_t w[64], char* padded_buffer, int i){
    
    uint32_t* aux_buffer = (uint32_t*) padded_buffer;

    for(int i = 0; i < 16; i++)
        w[i] = ntohl(aux_buffer[i]);

    for(int i = 16; i < 64; i++){
        uint32_t s0 = R_ROT(w[i -15],7) ^ R_ROT(w[i - 15], 18) ^ (w[i - 15] >> 3);
        uint32_t s1 = R_ROT(w[i -2],17) ^ R_ROT(w[i - 2], 19) ^ (w[i - 2] >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }

}

static void sum_h_prev_h(h_state *sha256_hash, h_state *sha256_prev){
    
    uint32_t *aux_hash = (uint32_t*) sha256_hash;
    uint32_t *aux_prev = (uint32_t*) sha256_prev;

    for(int i = 0; i < 8; i++)
        aux_hash[i] += aux_prev[i];

}

static char* endianness_sha(h_state *sha256_hash){
    uint32_t *sha_out =  (uint32_t*) calloc(8, sizeof(uint32_t));
    uint32_t *aux = (uint32_t*) sha256_hash;
    for(int i = 0; i < 8; i++)
        sha_out[i] = htonl(aux[i]);
    return (char*) sha_out;
}
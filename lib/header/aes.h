#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <arpa/inet.h>

enum {
    AES_128 = 4,
    AES_192 = 6,
    AES_256 = 8
};

enum { 
    EBC = 1,
    CBC = 2
};

size_t aes_encrypt(char* pass, char* string, size_t size, char** str_out, int mode, char* IV);
size_t aes_decrypt(char* pass, char* string, size_t size, char** str_out, int mode, char* IV);

char* _round_keys(char* key, int mode);
void _round_key_matrix(char key[4][4], char* keys, int init_word);
void _block_matrix(char key[4][4], char* keys, char size);
void _add_round_key(char block[4][4], char key[4][4]);
void _shift_rows(char block[4][4], int inv);
void _sub_bytes(char block[4][4], int inv);
void _mix_columns(char block[4][4], int inv);
char _cross_product(char block_byte, char matrix_byte);
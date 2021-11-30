#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#define AES_128 4
#define AES_192 6
#define AES_256 8

char* aes_encrypt(char* string, char* pass, int mode);
char* aes_decrypt(char* string, char* pass, int mode);

char* _round_keys(char* key, int mode);
void _round_key_matrix(char key[4][4], char* keys, int init_word);
void _round_key_matrix2(char key[4][4], char* keys, int init_word);
void _add_round_key(char block[4][4], char key[4][4]);
void _shift_rows(char block[4][4], int inv);
void _sub_bytes(char block[4][4], int inv);
void _mix_columns(char block[4][4], int inv);
char _cross_product(char block_byte, char matrix_byte);
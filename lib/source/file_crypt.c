#include "../header/file_crypt.h"

char* generate_iv(int size){
    char* iv = calloc(size,sizeof(char));
    srand(time(NULL));
    for(int i = 0; i < size; i++)
        iv[i] = rand() % 256;
    return iv;
}

void enc_file(struct file_crypt *data){
    char* buff = malloc(data->size * sizeof(char));
    char *iv = generate_iv(data->iv_sz);
    memcpy(buff, data->buff, data->size);
    fflush(stdout);
    aes_encrypt(data->pass, buff, data->size, &(data->buff), AES_128, iv);
    for(int i = 0; i < data->iv_sz; i++)
        putc( iv[i], data->file );
    for(int i = 0; i < ceil(data->size / (float)16) * 16 ; i++)
        putc( (data->buff)[i], data->file );
    fclose(data->file);
}

void dec_file(struct file_crypt *data){
    if(data->size == 0){
        fseek(data->file,0L, SEEK_END);
        data->size = ftell(data->file) - data->iv_sz;
        fseek(data->file,0L, SEEK_SET);
    }
    char *buff = malloc(data->size * sizeof(char));
    char *iv = calloc(data->iv_sz, sizeof(char));

    for(int i = 0; i < data->iv_sz; i++ )
        iv[i] = getc(data->file);
    for(int i = 0; i < data->size; i++ )
        buff[i] = getc(data->file);
    aes_decrypt( data->pass, buff, data->size, &(data->buff), AES_128, iv);
}
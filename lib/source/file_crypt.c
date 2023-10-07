#include "../header/file_crypt.h"

/**
 * @brief Generate random initial vector
 * 
 * @param size of initial vector
 * @return char* output string of initial vector
 */
static char* generate_iv(int size);

void File_Encrypt(struct file_crypt *data){

    //Copy buff from data
    char* buff = malloc(data->size * sizeof(char));
    memcpy(buff, data->buff, data->size);
    
    //Generate random initial vector
    char *iv = generate_iv(data->iv_sz);
    fflush(stdout);

    //Encrypt plain text using aes-128
    AES_Encrypt(data->pass, buff, data->size, &(data->buff), AES_128, iv);
    
    //Write initial vector to file
    for(int i = 0; i < data->iv_sz; i++)
        putc( iv[i], data->file );
    
    //Write encrypted buffer to file
    for(int i = 0; i < ceil(data->size / (float)16) * 16 ; i++)
        putc( (data->buff)[i], data->file );
    fclose(data->file);
}

void File_Decrypt(struct file_crypt *data){

    //Get encrypted data size
    if(data->size == 0){
        fseek(data->file,0L, SEEK_END);
        data->size = ftell(data->file) - data->iv_sz;
        fseek(data->file,0L, SEEK_SET);
    }

    //Get initial vector
    char *iv = calloc(data->iv_sz, sizeof(char));
    for(int i = 0; i < data->iv_sz; i++ )
        iv[i] = getc(data->file);

    //Decrypt file using aes-128
    char *buff = malloc(data->size * sizeof(char));
    for(int i = 0; i < data->size; i++ )
        buff[i] = getc(data->file);
    AES_Decrypt( data->pass, buff, data->size, &(data->buff), AES_128, iv);

}

static char* generate_iv(int size){
    
    //Allocate iv vector
    char* iv = calloc(size,sizeof(char));

    //Generate random string iv[size]
    srand(time(NULL));
    for(int i = 0; i < size; i++)
        iv[i] = rand() % 256;

    return iv;
}
#include "../header/history.h"
#include "../header/password_manager.h"
#include <stdlib.h>

List l_history = {
    .root = NULL,
    .size = 0,
    .initNode = _init_history,
    .cmpData = {NULL},
    .printNode = _print_history,
    .deleteNode = _delete_history,
    .sortNode = {NULL},
};

uint8_t addMessage(history_node* hist_nd){
    __ASSERT__(hist_nd != NULL, __ASSERT_NULL__)
    //Append message to list
    listInsert(&l_history,hist_nd,sizeof(history_node),LAST_POS);
}

uint8_t writeHistoryToFile(char* file_path, uint8_t isEncripted){
    
    //Open history file in write mode
    FILE* fd = fopen(file_path,"w");
    __ASSERT__(fd!=NULL, __ASSERT_NULL__);
    
    if(!isEncripted){
        //If option is plain text write history list to file
        for(uint16_t it = 0; it < l_history.size; it++){
            history_node* nd = listGetData(&l_history,it);
            fprintf(fd, "%s\x1%s\x1%s\x1%c\x1%s\n", nd->msg, nd->user, nd->group, nd->file_type, nd->file_path);
        }
        fclose(fd);
    }
    else{
        //If option is encrypted write to string and then on file
        //TODO: BUFFER HAS LIMITED SIZE, SHOULD BE DYNAMIC
        char* buff = calloc(2000, sizeof(char));
        uint16_t size_taken = 0;
        for(uint16_t it = 0; it < l_history.size; it++){
            history_node* nd = listGetData(&l_history,it);
            sprintf(buff+size_taken, "%s\x1%s\x1%s\x1%c\x1%s\n", nd->msg, nd->user, nd->group, nd->file_type, nd->file_path);
            size_taken += strlen(buff);
        }

        //Get encryption password
        password_nd* pass_node = getPassword(1,-1,NULL,0); 
        __ASSERT__(pass_node != NULL, __ASSERT_BACKTRACE__)
        char* password = pass_node->password;

        //Encrypt file
        struct file_crypt data = {
            .file = fd,
            .pass = password,
            .buff = buff,
            .size = size_taken,
            .iv_sz = 10,
        };
        enc_file(&data);

        //free buffer used to store file
        free(buff);
    }
}

void readHistoryFromFile(char* file_path, uint8_t isEncripted){
    
    //Open history file in read mode
    FILE* fd = fopen(file_path, "r");
    __ASSERT__(fd != NULL,__ASSERT_NULL__)

    //Get file encryption passord
    password_nd* pass_node = getPassword(1,-1,NULL,0); 
    __ASSERT__(pass_node != NULL, __ASSERT_BACKTRACE__)
    char* password = pass_node->password;

    //Decrypt file
    struct file_crypt data = {
        .file = fd,
        .pass = password,
        .buff = NULL,
        .size = 0,
        .iv_sz = 10
    };

    if(isEncripted)
        dec_file(&data);

    size_t size = 0;
    history_node nd = {
        .file_path = (char[100]){0},
        .group = (char[100]){0},
        .msg = (char[100]){0},
        .user = (char[100]){0},
        .file_type = 1,
        };
    
    //Get line from file and add it to history list
    int16_t line_size = 0;
    char* line = NULL, *out_buf = NULL;
    uint32_t size_line = 0, size_buf = 0;
    char* data_buff = data.buff;

    while(line_size != -1){
        //If encrypted get line from string using local function
        if(isEncripted){
            line_size = _get_line_string(&line, &size_line, data_buff, &out_buf, &data.size);
            data_buff = out_buf;
        }//If not encrypted get line from file using standard function
        else line_size = getline(&line,&size,fd);

        //Get paramaters for each member of the history node
        if(line_size != -1){
            sscanf(line,"%[^\x1]\x1%[^\x1]\x1%[^\x1]\x1%c\x1%[^\x1]",nd.msg, nd.user, nd.group, &nd.file_type, nd.file_path);
            listInsert(&l_history, &nd, sizeof(nd), LAST_POS);
            free(line);
        }
    }
    //Debug print list
    listPrint(&l_history, stdout);

}

void clearHistory(){
    listDeleteAll(&l_history);
}

static int _get_line_string(char** line, uint32_t* size_line, char* data_buf, char** out_buf, uint32_t* size_buf){
    __ASSERT__(data_buf != NULL,__ASSERT_NULL__)

    //Look for line feed or NULL character
    uint16_t i = 0;
    for(i = 0; data_buf[i] != '\0' && data_buf[i] != '\n' && (*size_buf); i++, (*size_buf)--);
    
    //If string's size is different from zero allocate string 
    if(i){
        (*line) = calloc( i + 1, sizeof(char));
        strncpy((*line), data_buf, i);
        (*line)[i] = '\0';
    }
    (*size_line) = i + 1;
    
    //Addresss after line 
    *out_buf = data_buf + i + 1;

    //If line's size is zero or reached end of line
    if(*size_buf && data_buf[i] != '\0')
        return 0;
    else{
        return -1;
    }
}

static void _delete_history(void* nd){
    history_node *aux = nd;
    free(aux->msg);
    free(aux->user);
    free(aux->group);
    free(aux->file_path);
    free(nd);
}

static void _print_history(FILE*f, void* nd){
	history_node *aux = nd;
	fprintf(f, "Msg : %s, User : %s, Group : %s, File type : %d, File path : %s\n",
    aux->msg, aux->user, aux->group, aux->file_type, aux->file_path);
}

static void _init_history(void* nd){
    history_node *aux = nd;
    aux->msg = strcpy((char*)malloc(strlen(aux->msg)), aux->msg);
    aux->user = strcpy((char*)malloc(strlen(aux->user)), aux->user);
    aux->group = strcpy((char*)malloc(strlen(aux->group)), aux->group);
    aux->file_path = strcpy((char*)malloc(strlen(aux->file_path)), aux->file_path);
}
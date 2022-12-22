#include "../header/password_manager.h"

enum {
    CMP_ID = 0,
    CMP_ID_TYPE,
};

List _password_list = {
    .root = NULL,
    .size = 0,
    .initNode = _init_password,
    .cmpData = {[CMP_ID] = _cmp_id, [CMP_ID_TYPE] = _cmp_id_type, NULL},
    .printNode = _print_password,
    .deleteNode = _delete_password,
    .sortNode = {NULL},
};


password_nd* getPassword(uint8_t id, uint8_t id_type, char* password, uint8_t pass_sz){

    __ASSERT__(_password_list.cmpData[CMP_ID] != NULL, __ASSERT_NULL__)
    __ASSERT__(_password_list.cmpData[CMP_ID_TYPE] != NULL, __ASSERT_NULL__)

    //Search id or idtype in list if they have a valid value
    if(id != -1)
        return listFind(&_password_list, NULL, &id, CMP_ID);
    else return listFind(&_password_list, NULL, &id, CMP_ID_TYPE);
}

void savePassword(password_nd* nd){
    listInsert(&_password_list, nd, sizeof(password_nd), LAST_POS);
}

void showPasswords(){
    fprintf(stdout, "PASSWORD LIST\n");
    listPrint(&_password_list, stdout);
}

void _init_password(void* nd){
    password_nd* aux = (password_nd*)nd;
    //Allocates and copies password
    char *pass_aux = NULL;
    memcpy( (pass_aux = malloc(aux->pass_sz)) , aux->password, aux->pass_sz);
    aux->password = pass_aux;
}

void _delete_password(void* nd){
    password_nd* aux = nd;
    free(aux->password);
}

void _print_password(FILE* f, void* nd){
    
    password_nd* aux = nd;
    fprintf(f, "PASSWORD SIZE: %d\n", aux->pass_sz);
    fprintf(f, "ID: %d\n", aux->id);

    fprintf(f, "STRING: ");
    for(uint8_t i = 0; i < aux->pass_sz; i++)
        fprintf(f, "%c", aux->password[i]);
    fprintf(f, "\n");

    fprintf(f, "HEX: ");
    for(uint8_t i = 0; i < aux->pass_sz; i++)
        fprintf(f, "0x%.2X ", aux->password[i]);
    fprintf(f, "\n");

}

int16_t _cmp_id(void* s_data, void* data){
    uint8_t id_searched = *(uint8_t *)s_data;
    uint8_t id = ((password_nd*)data)->id;
    return ! (id_searched == id);
}

int16_t _cmp_id_type(void* s_data, void* data){
    uint8_t id_type_searched = *(uint8_t *)s_data;
    uint8_t id_type = ((password_nd*)data)->type;
    return !(id_type_searched == id_type);
}
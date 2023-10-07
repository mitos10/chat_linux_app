#include "../header/password_manager.h"

/**
 * @brief Local function to init password node
 * 
 * @param nd data containing password's parameters
 */
static void init_password(void* nd);

/**
 * @brief Local function to delete password node
 * 
 * @param nd data containing password's parameters
 */
static void delete_password(void* nd);

/**
 * @brief Local function to print password
 * 
 * @param f output file
 * @param nd data containing password's parameters
 */
static void print_password(FILE* f, void* nd);

/**
 * @brief Local function to search id inside node
 * 
 * @param s_data searched data
 * @param data node data
 * @return int16_t 
 */
static int16_t cmp_id(void* s_data, void* data);

/**
 * @brief Local function to search type inside node
 * 
 * @param s_data searched data
 * @param data node data
 * @return int16_t 
 */
static int16_t cmp_id_type(void* s_data, void* data);

enum {
    CMP_ID = 0,
    CMP_ID_TYPE,
};

List _password_list = {
    .root = NULL,
    .size = 0,
    .initNode = init_password,
    .cmpData = {[CMP_ID] = cmp_id, [CMP_ID_TYPE] = cmp_id_type, NULL},
    .printNode = print_password,
    .deleteNode = delete_password,
    .sortNode = {NULL},
};

password_nd* Password_get(uint8_t id, uint8_t id_type, char* password, uint8_t pass_sz){

    __ASSERT__(_password_list.cmpData[CMP_ID] != NULL, __ASSERT_NULL__)
    __ASSERT__(_password_list.cmpData[CMP_ID_TYPE] != NULL, __ASSERT_NULL__)

    //Search id or idtype in list if they have a valid value
    if(id_type == (uint8_t)-1)
        return List_Find(&_password_list, NULL, &id, CMP_ID);
    else return List_Find(&_password_list, NULL, &id, CMP_ID_TYPE);
}

void Password_save(password_nd* nd){
    List_Insert(&_password_list, nd, sizeof(password_nd), LAST_POS);
}

void Password_show(){
    fprintf(stdout, "PASSWORD LIST\n");
    List_Print(&_password_list, stdout);
}

static void init_password(void* nd){
    password_nd* aux = (password_nd*)nd;
    //Allocates and copies password
    char *pass_aux = NULL;
    memcpy( (pass_aux = malloc(aux->pass_sz)) , aux->password, aux->pass_sz);
    aux->password = pass_aux;
}

static void delete_password(void* nd){
    password_nd* aux = nd;
    free(aux->password);
}

static void print_password(FILE* f, void* nd){
    
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

int16_t cmp_id(void* s_data, void* data){
    uint8_t id_searched = *(uint8_t *)s_data;
    uint8_t id = ((password_nd*)data)->id;
    return ! (id_searched == id);
}

int16_t cmp_id_type(void* s_data, void* data){
    uint8_t id_type_searched = *(uint8_t *)s_data;
    uint8_t id_type = ((password_nd*)data)->type;
    return !(id_type_searched == id_type);
}
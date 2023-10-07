#include "../../lib/header/list.h"
#include "../../lib/header/debug.h"
#include <string.h>

void _stringLprint(FILE* f, void* data){
    fprintf(f, "%s\n", (char*)data);
}

int16_t _stringLcmp(void* s1, void* s2){
    return strcmp(s1, s2);
}

void _stringLdel(void* data){
    free(data);
}

void _stringLsortBubble(List* l, uint16_t func_cmp) {
    uint8_t is_sorted = 0;
    while(!is_sorted){
        is_sorted = 1;
        for(size_t i = 0; i < l->size - 1; i++){
            int cmp = l->cmpData[func_cmp](List_GetData(l,i), List_GetData(l,i+1));
            if(cmp > 0){
                List_Move(l,i+1,i);
                is_sorted = 0;
            }
        }
    }

}

int main(){

    Memory_DefaultInit(100000);
    
    List stringL = {
        .root = NULL,
        .size = 0,
        .printNode = _stringLprint,
        .deleteNode = _stringLdel,
        .cmpData = { _stringLcmp, NULL},
        .sortNode = { _stringLsortBubble, NULL},
    };

    List_Insert(&stringL, "hi", sizeof("hi"), LAST_POS);
    List_Insert(&stringL, "what", sizeof("what"), LAST_POS);
    List_Insert(&stringL, "is", sizeof("is"), LAST_POS);
    List_Insert(&stringL, "this", sizeof("this"), LAST_POS);

    List_Print(&stringL, stdout);
    printf("\n");

    List_Swap(&stringL, 0, 3);

    for(size_t i = 0; i < List_GetSize(&stringL); i++)
        printf("%ld. %s\n", i+1, (char*)List_GetData(&stringL, i));

    printf("\n");

    uint16_t pos = 0;
    char* str = List_Find(&stringL, &pos, "is", 0);

    printf("Search function.\nPos : %d, Str: %s, Str from pos: %s\n\n", pos+1, str, (char*)List_GetData(&stringL,pos));
    
    for(int i = 0; List_GetSize(&stringL); i++){
        List_Print(&stringL, stdout);
        List_Delete(&stringL, LAST_POS);
        printf("\n");
    }

    List_DeleteAll(&stringL);

    List_Insert(&stringL, "hi", sizeof("hi"), LAST_POS);
    List_Insert(&stringL, "what", sizeof("what"), LAST_POS);
    List_Insert(&stringL, "is", sizeof("is"), LAST_POS);
    List_Insert(&stringL, "this", sizeof("this"), LAST_POS);

    List_Sort(&stringL,0,0);

    for(int i= 0; List_GetSize(&stringL); i++){
        List_Print(&stringL, stdout);
        List_Delete(&stringL,0);
        printf("\n");
    }

    return 0;
}
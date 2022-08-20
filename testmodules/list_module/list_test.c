#include "../../lib/header/list.h"
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
        for(int i = 0; i < l->size - 1; i++){
            int cmp = l->cmpData[func_cmp](listGetData(l,i), listGetData(l,i+1));
            if(cmp > 0){
                listMove(l,i+1,i);
                is_sorted = 0;
            }
        }
    }

}

void main(){
    
    List stringL = {
        .root = NULL,
        .size = 0,
        .printNode = _stringLprint,
        .deleteNode = _stringLdel,
        .cmpData = { _stringLcmp, NULL},
        .sortNode = { _stringLsortBubble, NULL},
    };

    listInsert(&stringL, "hi", sizeof("hi"), LAST_POS);
    listInsert(&stringL, "what", sizeof("what"), LAST_POS);
    listInsert(&stringL, "is", sizeof("is"), LAST_POS);
    listInsert(&stringL, "this", sizeof("this"), LAST_POS);

    listPrint(&stringL, stdout);
    printf("\n");

    listSwap(&stringL, 0, 3);

    for(int i = 0; i < listGetSize(&stringL); i++)
        printf("%d. %s\n", i+1, (char*)listGetData(&stringL, i));

    printf("\n");

    uint16_t pos = 0;
    char* str = listFind(&stringL, &pos, "is", 0);

    printf("Search function.\nPos : %d, Str: %s, Str from pos: %s\n\n", pos+1, str, (char*)listGetData(&stringL,pos));
    
    for(int i = 0; listGetSize(&stringL); i++){
        listPrint(&stringL, stdout);
        listDelete(&stringL, LAST_POS);
        printf("\n");
    }

    listDeleteAll(&stringL);

    listInsert(&stringL, "hi", sizeof("hi"), LAST_POS);
    listInsert(&stringL, "what", sizeof("what"), LAST_POS);
    listInsert(&stringL, "is", sizeof("is"), LAST_POS);
    listInsert(&stringL, "this", sizeof("this"), LAST_POS);

    listSort(&stringL,0,0);

    for(int i= 0; listGetSize(&stringL); i++){
        listPrint(&stringL, stdout);
        listDelete(&stringL,0);
        printf("\n");
    }


}
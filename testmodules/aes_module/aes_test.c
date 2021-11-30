#include "../../lib/aes.h"

int main(){

    char* aux = aes_encrypt( "Thats my Kung Fu", "Two One Nine TwoTwo One Nine TwoCosasne threeTwo", AES_128);
    aes_decrypt( "Thats my Kung Fu", aux, AES_128);
    
}
#include "../../lib/aes.h"

int main(){

    char* aux = AES_Encrypt( "Thats my Kung Fu", "Two One Nine TwoTwo One Nine TwoCosasne threeTwo", AES_128);
    AES_Decrypt( "Thats my Kung Fu", aux, AES_128);
    
}
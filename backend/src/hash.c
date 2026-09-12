#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <openssl/sha.h>
#include <ctype.h>
#include "hash.h"


int verify_password(const char *password, const char *hash) {

    int aux = -1;

    if(strcmp(password, hash) == 0){
        aux = 0;
    }else{
        aux = 1;
    }

    return aux;
}

void hash_password(const char *password, const char *salt, char *out_hash) {
    char buf[128];
    snprintf(buf, sizeof(buf), "%s%s", password, salt);
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1((unsigned char*)buf, strlen(buf), hash);

    for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
        sprintf(out_hash + i*2, "%02x", hash[i]);
}


void url_decode(char *dst, const char *src) {
    char a, b;
    while (*src) {

        if ((*src == '%') && ((a = src[1]) && (b = src[2])) && (isxdigit(a) && isxdigit(b))){

            if(a >= 'a'){
                a -= 'a'-'A';
            }
            if(a >= 'A'){
                a -= ('A' - 10);
            }else{
                a -= '0';
            }

            if(b >= 'a'){
                b -= 'a'-'A';
            }
            if(b >= 'A'){
                b -= ('A' - 10);
            }else{
                b -= '0';
            }

            *dst++ = 16*a+b;
             src+=3;

        } else if (*src == '+') {
            *dst++ = ' ';
            src++;
        } else {
            *dst++ = *src++;
        }
    }
    *dst++ = '\0';
}

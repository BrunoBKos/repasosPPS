#include "comp.h"

/* comprime */

int comprime(FILE* fp) {
    char* res; 
    char* aux;
    int j,tam,ok;
    res = (char*) malloc(sizeof(char)*2048);
    if(res == NULL) {
        return 1;  
    }
    tam = 0;
    j = 0;
    aux = res;
    ok = 0;
    while(ok == 0) {
        tam = fread(aux,1,2048,fp);
        if(tam != 2048) {
            ok = 1;
        }
        j = j + tam;
        res = (char*) realloc(res, sizeof(char)*(j+2048));
        if(res == NULL) {
            ok = 1;
        }
        aux = res + j;
    }
    if(res == NULL) {
        return 1;
    }
    fprintf(stdout,"%ld\n",strlen(res));
    return 0;
}

/* descomprime */

int descomprime(FILE* fp) {
    if ( fp == NULL) {
        return 1;
    }
    return 0;
}
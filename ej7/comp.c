#include "comp.h"

/* comprime */

int comprime(char* src, char* dest) {
    char* res; 
    char* aux;
    char* mbits;
    unsigned char byte;
    int integral[2];
    int k, i, j,tam,ok,mask;
    FILE* fp;
    fp = fopen(src,"r");
    if(fp == NULL) {
        return 1;
    }
    res = (char*) malloc(sizeof(char)*2048);
    if(res == NULL) {
        fclose(fp);
        return 1;  
    }
    tam = 0;
    j = 0;
    aux = res;
    ok = 0;
    while(ok == 0) {
        tam = fread(aux,1,2048,fp);
        j = j + tam;
        if(tam != 2048) {
            ok = 1;
        }
        else {
            res = (char*) realloc(res, sizeof(char)*(j+2048));
            if(res == NULL) {
                ok = 1;
            }
            else {
                aux = res + j;
            }
        }
    }
    fclose(fp);
    if(res == NULL) {
        return 1;
    }
    res = (char*) realloc(res,sizeof(char)*j);
    if(res == NULL) {
        return 1;
    }
    tam = j;
    aux = (char*) malloc(sizeof(char)*(j + j/8 + 5));
    if(aux == NULL) {
        free(res);
        return 1;
    }
    fp = fopen(dest, "w");
    if(fp == NULL) {
        free(res);
        free(aux);
        return 1;
    }
    mbits = aux;
    mbits[0] = (unsigned char) (tam / 256);
    mbits[1] = (unsigned char) (tam % 256);
    mbits[2] = (unsigned char) 1;
    aux = aux + tam/8 + 5;
    k = 5;
    for(i = 0; i < 8; i++) {
        aux[i] = res[i];
    }
    j = i;
    mask = 128;
    byte = 0;
    while(i < tam) {
        if(mask == 0) {
            mask = 128;
            mbits[k] = byte;
            byte = 0;
            k++;
        }
        integral[1] = buscaMax(res,i,tam,integral);
        if(integral[1] < 5){
            aux[j] = res[i];
            i++;
            j++;
        }
        else {
            aux[j+0] = (unsigned char) (integral[0] / 256);
            aux[j+1] = (unsigned char) (integral[0] % 256);
            aux[j+2] = (unsigned char) (integral[1] / 256);
            aux[j+3] = (unsigned char) (integral[1] % 256);
            i += integral[1];
            j += 4;
            byte += mask;
        }
        mask = mask>>1;
    }
    mbits[k] = byte;
    k++;
    mbits[3] = (unsigned char) (k / 256);
    mbits[4] = (unsigned char) (k % 256);
    for(i = 0; i < j; i++) {
        mbits[k+i] = aux[i];
    }
    /*for(i = 0; i < (j+k); i++) {
        printf("%x ",mbits[i]);
    }*/
    fwrite(mbits,1,j+k,fp);
    free(res);
    free(mbits);
    fclose(fp);
    return 0;
}

/* descomprime */

int descomprime(char* src, char* dest) {
    char* res; 
    char* aux;
    char* mbits;
    int j,tam,ok,mask,i,k;
    int integral[2];
    FILE* fp;
    fp = fopen(src,"r");
    if(fp == NULL) {
        return 1;
    }
    res = (char*) malloc(sizeof(char)*2048);
    if(res == NULL) {
        fclose(fp);
        return 1;  
    }
    tam = 0;
    j = 0;
    aux = res;
    ok = 0;
    while(ok == 0) {
        tam = fread(aux,1,2048,fp);
        j = j + tam;
        if(tam != 2048) {
            ok = 1;
        }
        else {
            res = (char*) realloc(res, sizeof(char)*(j+2048));
            if(res == NULL) {
                ok = 1;
            }
            else {
                aux = res + j;
            }
        }
    }
    fclose(fp);
    if(res == NULL) {
        return 1;
    }
    res = (char*) realloc(res,sizeof(char)*j);
    if(res == NULL) {
        return 1;
    }
    ok = j;
    tam = (int) ((unsigned char) res[1] + ((unsigned char) res[0])*256);
    mbits = res + (int) ((unsigned char) res[4] + ((unsigned char) res[3])*256);
    aux = (char*) malloc(sizeof(char)*(tam));
    if(aux == NULL) {
        free(res);
        return 1;
    }
    fp = fopen(dest, "w");
    if(fp == NULL) {
        free(res);
        free(aux);
        return 1;
    }
    mask = 128;
    for(i = 0; i < 8; i++) {
        aux[i] = mbits[i];
    }
    j = i;
    k = 5;
    while(i < tam && j < ok) {
        if(mask == 0) {
            mask = 128;
            k++;
        }
        if(!((unsigned char) mask & (unsigned char) res[k])) {
            aux[i] = mbits[j];
            i++;
            j++;
        }
        else {
            integral[0] = (int) ((unsigned char) mbits[j]);
            integral[0] = integral[0]*256 + (unsigned char) mbits[j+1];
            integral[1] = (int) ((unsigned char) mbits[j+2]);
            integral[1] = integral[1]*256 + (unsigned char) mbits[j+3];
            while(integral[1] > 0) {
                aux[i] = aux[integral[0]];
                integral[0] += 1;
                i++;
                integral[1] -= 1;
            }
            j+=4;
        }
        mask = mask>>1;
    }
    fwrite(aux, 1, i, fp);
    free(aux);
    free(res);
    return 0;
}

int coinCar(char* str,char* str2, int to) {
    int res;
    res = 0;
    while(res < to) {
        if(str[res] != str2[res]) {
            break;
        }
        res++;
    }
    return res;
}

int buscaMax(char* str, int from, int to, int* pos) {
    int i,aux;
    int res[2];
    res[0] = 0;
    i = 0;
    while(i < from) {
        if(str[from] == str[i]) {
            aux = coinCar(&str[from], &str[i],to - from);
            if(res[0] < aux) {
                res[0] = aux;
                res[1] = i;
            }
        }
        i++;
    }
    *pos = res[1];
    return res[0];
}
